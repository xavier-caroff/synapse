///
/// @file Manager.cpp
///
/// Implementation of the Manager class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <latch>
#include <memory>
#include <regex>
#include <stdexcept>
#include <thread>

#include <boost/dll.hpp>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include "synapse/framework/Dispatcher.h"
#include "synapse/framework/IConsumer.h"
#include "synapse/framework/IProducer.h"
#include "synapse/framework/IRunnable.h"
#include "synapse/framework/Manager.h"

namespace synapse {
namespace framework {

// Constructor.
Manager::Manager()
{
}

// Destructor.
Manager::~Manager()
{
}

// Create a block from its class name.
IBlock* Manager::create(
	const std::string& name,
	const std::string& className)
{
	// Check the name is valid.
	if (!isValidName(name))
	{
		throw std::runtime_error(fmt::format("'{}' is not a valid block name", name));
	}

	// Check if name is not already used.
	if (_blocks.find(name) != _blocks.end())
	{
		throw std::runtime_error("another existing block has the same name");
	}

	// Find the description.
	const Registry::BlockDescription& description = _registry.find(className);
	IBlock*                           block       = description._create(name);

	_blocks.emplace(name, block);

	return block;
}

// Find a block from its name.
IBlock* Manager::find(
	const std::string& name) const
{
	auto    itr    = _blocks.find(name);
	IBlock* result = nullptr;

	if (itr != _blocks.end())
	{
		result = itr->second;
	}

	return result;
}

// Find a port from its block and class name.
IPort* Manager::find(
	IBlock*            block,
	const std::string& name)
	const
{
	auto itr = std::find_if(_ports.cbegin(), _ports.cend(), [block, name](const auto& current) {
		return current->block() == block && current->name() == name;
	});

	if (itr == _ports.cend())
	{
		throw std::logic_error(fmt::format("port '{}' not found for block '{}'", name, block->name()).c_str());
	}

	return itr->get();
}

// Initialize the object from configuration data.
void Manager::initialize(
	const ConfigData& config)
{
	// Load modules.
	loadModules(config);

	// Create the blocks.
	createBlocks(config);

	// Create the routes.
	createRoutes(config);

	// Initialize the blocks.
	initializeBlocks(config);
}

// Start the blocks and wait for terminaison request.
void Manager::run()
{
	// Start the runnables block running in a dedicated thread.
	size_t runnableCount =
		std::count_if(_blocks.cbegin(), _blocks.cend(), [](const auto& current) { return dynamic_cast<IRunnable*>(current.second); }) +
		_dispatchers.size();
	std::latch               latch(runnableCount);
	std::vector<std::thread> runnables;
	auto                     run = [](IRunnable* runnable, std::latch& latch) {
        // Execute the 	runnable
        runnable->run();

        // Log the end of the runnable
        std::string type;
        std::string name;
        if (auto block = dynamic_cast<IBlock*>(runnable))
        {
            type = "Block";
            name = block->name();
        }
        else if (auto dispatcher = dynamic_cast<Dispatcher*>(runnable))
        {
            type = "Dispatcher";
            name = dispatcher->name();
        }
        spdlog::info("{} '{}' terminated", type, name);

        // Signal the termination
        latch.count_down();
    };

	for (auto& current : _dispatchers)
	{
		if (auto runnable = dynamic_cast<IRunnable*>(current.second.get()))
		{
			runnables.emplace_back(std::thread(run, runnable, std::ref(latch)));
		}
	}
	for (auto& current : _blocks)
	{
		if (auto runnable = dynamic_cast<IRunnable*>(current.second))
		{
			runnables.emplace_back(std::thread(run, runnable, std::ref(latch)));
		}
	}

	// Wait for runnables to finish.
	std::this_thread::yield();
	latch.wait();
	for (auto& thread : runnables)
	{
		thread.join();
	}

	// Delete blocks.
	for (auto& current : _blocks)
	{
		current.second->destroy();
		current.second = nullptr;
	}
}

// Ask to stop the execution.
void Manager::shutdown()
{
	// Ask the blocks to shutdown.
	for (auto& current : _blocks)
	{
		current.second->shutdown();
	}

	// Ask the dispatchers to shutdown.
	for (auto& current : _dispatchers)
	{
		current.second->shutdown();
	}
}

// Check the provided name is a valid name for block, route and port.
bool Manager::isValidName(
	const std::string& name)
{
	std::regex pattern("^[a-z][a-z0-9-]*$");

	return std::regex_match(name, pattern);
}

// Load all possible modules.
void Manager::loadModules(
	const ConfigData& config)
{
	std::filesystem::path exe       = boost::dll::program_location().string();
	std::filesystem::path exeFolder = exe.parent_path();

	// Load modules installed in the same folder as the executable.
	loadModules(exeFolder);

	// Load the modules located in the additional folders of the configuration file.
	if (config.find("additionalPackageFolders") != config.end())
	{
		for (const auto& current : config.at("additionalPackageFolders"))
		{
			if (current.is_string())
			{
				std::filesystem::path path{ current.get<std::string>() };

				if (path.is_relative())
				{
					path = exeFolder / path;
				}

				loadModules(path);
			}
		}
	}
}

// Load the external modules located into a given folder.
void Manager::loadModules(
	const std::filesystem::path& folder)
{
	for (const auto& current : std::filesystem::directory_iterator(folder))
	{
		if (current.path().extension() == std::filesystem::path{ ".so" } ||
			current.path().extension() == std::filesystem::path{ ".dll" } ||
			current.path().extension() == std::filesystem::path{ ".dylib" })
		{
			try
			{
				boost::dll::shared_library lib(current.path().string());
				auto                       registerBlocks = lib.get<void(Registry&)>(Registry::ENTRY_POINT_FUNCTION);

				registerBlocks(_registry);

				_modules.push_back(std::move(lib));
				spdlog::info("Module {} loaded", current.path().string());
			}
			catch (boost::system::system_error&)
			{
				// can't find the entry point, not a new-nav module :-(.
			}
		}
	}
}

// Create the blocks described into the configuration file.
void Manager::createBlocks(
	const ConfigData& config)
{
	for (const auto& current : config.at("blocks"))
	{
		std::string name      = current.at("name").get<std::string>();
		std::string className = current.at("className").get<std::string>();
		IBlock*     block     = nullptr;

		// Instantiate the object.
		try
		{
			block = this->create(name, className);
		}
		catch (std::runtime_error& e)
		{
			throw std::runtime_error(fmt::format("failed to create block {}: {}", name, e.what()));
		}

		// Instantiate the output ports
		if (auto producer = dynamic_cast<IProducer*>(block))
		{
			std::map<std::string, bool> ports;

			for (auto& portName : producer->ports(current.at("config")))
			{
				// Check the name is valid.
				if (!isValidName(portName))
				{
					block->destroy();
					block = nullptr;
					throw std::runtime_error(fmt::format("'{}' is not a valid port name (block class '{}')", portName, className));
				}

				// Check the port name is not already used.
				if (ports.find(portName) != ports.end())
				{
					block->destroy();
					block = nullptr;
					throw std::logic_error(fmt::format("block `{}`: another existing port has the same name `{}`", block->name(), portName));
				}

				_ports.emplace_back(std::make_unique<Port>(portName, block));
				ports[portName] = true;
			}
		}
	}
}

// Create the routes described into the configuration file.
void Manager::createRoutes(
	const ConfigData& config)
{
	int counter = 0;

	for (const auto& current : config.at("routes"))
	{
		++counter;

		// Get attributes from json.
		auto        sources      = current.at("sources").get<std::list<std::string>>();
		auto        destinations = current.at("destinations").get<std::list<std::string>>();
		std::string name;

		// Check the name is not empty.
		if (current.find("name") != current.end())
		{
			name = current.at("name").get<std::string>();
			if (name.empty())
			{
				throw std::runtime_error(fmt::format("route '#{}': name cannot be empty", counter));
			}

			// Check the name is valid.
			if (!isValidName(name))
			{
				throw std::runtime_error(fmt::format("'{}' is not a valid route name", name));
			}
		}
		std::string errName = name.empty() ? fmt::format("unnamed #{}", counter) : name;

		// Check that sources and destinations are not empty.
		if (sources.empty() || destinations.empty())
		{
			throw std::runtime_error(fmt::format("route '{}': sources and destinations shall not be empty", errName));
		}

		// Prepare the list of source ports.
		auto prepareSources = [this, errName](std::list<std::string>& names) {
			std::list<Port*> result;

			for (const auto& current : names)
			{
				// Find the name of the block and the name of the port.
				std::string blockName = current;
				std::string portName;
				auto        pos = current.find('.');

				if (pos != std::string::npos)
				{
					blockName = current.substr(0, pos);
					portName  = current.substr(pos + 1);

					if (portName.empty())
					{
						throw std::runtime_error(fmt::format("route '{}': port name shall not be empty in '{}'", errName, current));
					}
				}

				if (blockName.empty())
				{
					throw std::runtime_error(fmt::format("route '{}': block name shall not be empty in '{}'", errName, current));
				}

				// Find the block.
				auto itrBlock = _blocks.find(blockName);

				if (itrBlock == _blocks.end())
				{
					throw std::runtime_error(fmt::format("route '{}': block '{}' not found in the definition of a route", errName, blockName));
				}

				auto block = itrBlock->second;

				// Find the port name if no port name is provided.
				if (portName.empty())
				{
					// Scan the ports and check there is only one port for this block.
					size_t count{ 0 };
					for (const auto& port : _ports)
					{
						if (port->block() == block)
						{
							++count;
							portName = port->name();
						}
					}
					if (count > 1)
					{
						throw std::runtime_error(fmt::format("route '{}': block '{}' has more than one port, port name shall be provided", errName, blockName));
					}
				}

				// Find the port.
				auto itrPort = std::find_if(
					_ports.cbegin(),
					_ports.cend(),
					[&](const std::unique_ptr<Port>& p) { return p->name() == portName && p->block() == block; });

				if (itrPort == _ports.cend())
				{
					throw std::runtime_error(fmt::format("route '{}': port '{}' not found in the definition of a route", errName, current));
				}

				result.push_back(itrPort->get());
			}

			return result;
		};

		// Prepare the list of destination blocks.
		auto prepareDestinations = [this, errName](std::list<std::string>& names) {
			std::list<IConsumer*> result;

			for (const auto& current : names)
			{
				auto itr = _blocks.find(current);

				if (itr == _blocks.end())
				{
					throw std::runtime_error(fmt::format("route '{}': block '{}' not found in the definition of a route", errName, current));
				}
				if (auto consumer = dynamic_cast<IConsumer*>(itr->second))
				{
					result.push_back(consumer);
				}
				else
				{
					throw std::runtime_error(fmt::format("route '{}': block '{}' is not a consumer", errName, current));
				}
			}

			return result;
		};

		auto sourcePorts       = prepareSources(sources);
		auto destinationBlocks = prepareDestinations(destinations);

		// When the route is named, check it is unique.
		if (!name.empty() && _namedRoutes.find(name) != _namedRoutes.end())
		{
			throw std::runtime_error(fmt::format("route '{}' is already defined", name));
		}

		// Create or find the dispatcher.
		Dispatcher* dispatcher = nullptr;
		{
			// Get the name of the dispatcher.
			static const std::string DEFAULT_DISPATCHER_NAME = "default";
			std::string              dispatcherName{ DEFAULT_DISPATCHER_NAME };

			if (current.find("dispatcher") != current.end())
			{
				dispatcherName = current.at("dispatcher").get<std::string>();
				if (dispatcherName == DEFAULT_DISPATCHER_NAME)
				{
					throw std::runtime_error(fmt::format("route '{}': dispatcher name shall not be '{}'", name, DEFAULT_DISPATCHER_NAME));
				}
			}

			// Create the dispatcher or get a pointer to it.
			auto itr = _dispatchers.find(dispatcherName);

			if (itr == _dispatchers.end())
			{
				dispatcher = _dispatchers.emplace(dispatcherName, std::make_unique<Dispatcher>(dispatcherName)).first->second.get();
			}
			else
			{
				dispatcher = itr->second.get();
			}
		}

		// Create the route.
		auto& route = _routes.emplace_back(std::make_unique<Route>(sourcePorts, destinationBlocks, dispatcher));

		// register it with its name eventually.
		if (!name.empty())
		{
			_namedRoutes.emplace(name, _routes.back().get());
		}

		// Register the route to its source ports.
		for (auto& sourcePort : sourcePorts)
		{
			sourcePort->attach(route.get());
		}
	}
}

// Initialize the blocks.
void Manager::initializeBlocks(
	const ConfigData& config)
{
	for (const auto& current : config["blocks"])
	{
		std::string name = current.at("name").get<std::string>();

		try
		{
			_blocks.find(name)->second->initialize(current.at("config"), this);
		}
		catch (std::runtime_error& e)
		{
			throw std::runtime_error(fmt::format("failed to create block {}: {}", name, e.what()));
		}
	}
}

} // namespace framework
} // namespace synapse
