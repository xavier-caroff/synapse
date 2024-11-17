///
/// @file Nmea0183RouterFiber.cpp
///
/// Implementation of the Nmea0183RouterFiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <regex>
#include <string>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include "Nmea0183RouterFiber.h"

namespace synapse {
namespace modules {
namespace marine {

IMPLEMENT_BLOCK(Nmea0183RouterFiber)

/// Conversion of a json object to a Framer::Config::Route object.
///
/// @param json The json representation of the object's content.
/// @param object The object to initialize.
static void from_json(
	const nlohmann::json&               json,
	Nmea0183RouterFiber::Config::Route& object)
{
	object.port     = json.at("port").get<std::string>();
	object.patterns = json.at("patterns").get<std::vector<std::string>>();
}

/// Conversion of a json object to a Framer::Config object.
///
/// @param json The json representation of the object's content.
/// @param object The object to initialize.
static void from_json(
	const nlohmann::json&        json,
	Nmea0183RouterFiber::Config& object)
{
	object.routes = json.at("routes").get<std::vector<Nmea0183RouterFiber::Config::Route>>();

	if (json.find("fallback") != json.end())
	{
		object.fallback = json.at("fallback").get<std::string>();
	}
	else
	{
		object.fallback.reset();
	}
}

// Constructor.
Nmea0183RouterFiber::Nmea0183RouterFiber(
	const std::string& name)
	: Fiber(name)
{
}

// Destructor.
Nmea0183RouterFiber::~Nmea0183RouterFiber()
{
	shutdown();
}

// Initialize the block before the execution.
void Nmea0183RouterFiber::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
	// Get configuration data.
	_config = readConfig<Nmea0183RouterFiber::Config>(configData);

	// Prepare the collection giving the port depending on the pattern.
	std::map<std::string, std::string> ports;

	for (const auto& route : _config.routes)
	{
		for (const auto& pattern : route.patterns)
		{
			auto itr = ports.find(pattern);
			if (itr == ports.end())
			{
				ports[pattern] = route.port;
			}
			else if (itr->second != route.port)
			{
				throw std::runtime_error(fmt::format("pattern {} is already used by port {}", pattern, itr->second).c_str());
			}
		}
	}

	// If no route defined, throw an exception.
	if (ports.empty())
	{
		throw std::runtime_error(fmt::format("no route defined for block {}", name()).c_str());
	}

	// Create the tree.
	for (const auto& item : ports)
	{
		// Find the port.
		auto port = manager->find(this, item.second);

		if (!_root)
		{
			_root = std::make_unique<Node>();
			_root->initialize(reinterpret_cast<const uint8_t*>(item.first.data()), item.first.size(), port);
		}
		else
		{
			_root->extend(reinterpret_cast<const uint8_t*>(item.first.data()), item.first.size(), port);
		}
	}

	// Initialize the fallback port.
	_fallback = _config.fallback ? manager->find(this, *_config.fallback) : nullptr;
}

// Ask the component to prepare to be deleted (terminate all pending operations).
void Nmea0183RouterFiber::shutdown()
{
}

// Get the list of output ports.
std::list<std::string> Nmea0183RouterFiber::ports(
	const IBlock::ConfigData& configData)
{
	std::list<std::string> result;
	auto                   config = readConfig<Nmea0183RouterFiber::Config>(configData);

	if (config.routes.empty())
	{
		throw std::runtime_error(fmt::format("no route defined for block {}", name()).c_str());
	}

	if (config.fallback)
	{
		result.push_back(*config.fallback);
	}

	for (const auto& route : config.routes)
	{
		if (!route.patterns.empty() &&
			std::find(result.cbegin(), result.cend(), route.port) == result.cend())
		{
			result.push_back(route.port);
		}
	}

	return result;
}

// Consume a message.
void Nmea0183RouterFiber::consume(
	const std::shared_ptr<synapse::framework::Message>& message)
{
	// Check if the message matches one of the routes.
	auto port = _root->match(message->payload(), message->size());

	// If not, use the fallback port.
	if (port == nullptr)
	{
		port = _fallback;
	}

	// Send the message to the port.
	if (port)
	{
		port->dispatch(message);
	}
}

// Initialize the node.
void Nmea0183RouterFiber::Node::initialize(
	const uint8_t              bytes[],
	size_t                     size,
	synapse::framework::IPort* port_)
{
	if (port_ == nullptr)
	{
		throw std::logic_error("Nmea0183RouterFiber::Node::initialize shall not be called with port == nullptr");
	}

	if (size == 1)
	{
		byte           = bytes[0];
		this->next     = nullptr;
		this->fallback = nullptr;
		this->port     = port_;
	}
	else if (size > 1)
	{
		byte       = bytes[0];
		this->next = std::make_unique<Node>();
		this->next->initialize(std::addressof(bytes[1]), size - 1, port_);
		this->fallback = nullptr;
		this->port     = nullptr;
	}
	else
	{
		throw std::logic_error("Nmea0183RouterFiber::Node::initialize shall not be called with size == 0");
	}
}
// Extend the chain from this node.
void Nmea0183RouterFiber::Node::extend(
	const uint8_t              bytes[],
	size_t                     size,
	synapse::framework::IPort* port_)
{
	if (port_ == nullptr)
	{
		throw std::logic_error("Nmea0183RouterFiber::Node::extend shall not be called with port == nullptr");
	}

	if (size == 0)
	{
		throw std::runtime_error("Nmea0183RouterFiber::Node::extend shall not be called with size == 0");
	}

	// Create a new chain if the first byte of the new pattern does not match the current one.
	if (bytes[0] != byte)
	{
		if (!fallback)
		{
			fallback = std::make_unique<Node>();
			fallback->initialize(bytes, size, port_);
		}
		else
		{
			fallback->extend(bytes, size, port_);
		}
	}
	// Extend the chain if the first byte of the new pattern match the current one.
	else
	{
		if (this->port != nullptr)
		{
			throw std::runtime_error("Nmea0183RouterFiber::Node::extend can't extend from a terminal node");
		}

		if (size == 1)
		{
			throw std::runtime_error("Nmea0183RouterFiber::Node::extend can't extend with a sub-string");
		}

		next->extend(std::addressof(bytes[1]), size - 1, port_);
	}
}

// Check if the begining of the provided data matches one of the patterns of this node.
synapse::framework::IPort* Nmea0183RouterFiber::Node::match(
	const uint8_t bytes[],
	size_t        size) const
{
	synapse::framework::IPort* result{ nullptr };

	if (size != 0)
	{
		if (bytes[0] == this->byte)
		{
			if (this->next.get() != nullptr)
			{
				result = this->next->match(std::addressof(bytes[1]), size - 1);
			}
			else
			{
				result = this->port;
			}
		}
		else
		{
			if (this->fallback.get() != nullptr)
			{
				result = this->fallback->match(bytes, size);
			}
		}
	}

	return result;
}

} // namespace marine
} // namespace modules
} // namespace synapse
