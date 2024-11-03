///
/// @file Manager.h
///
/// Declaration of the Manager class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <filesystem>
#include <list>
#include <map>
#include <memory>
#include <string>

#include <boost/dll.hpp>

#include <nlohmann/json.hpp>

#include <synapse/framework/Dispatcher.h>
#include <synapse/framework/IManager.h>
#include <synapse/framework/Port.h>
#include <synapse/framework/Registry.h>
#include <synapse/framework/Route.h>

namespace synapse {
namespace framework {

///
/// The block manager manage blocks.
///
class Manager :
	public IManager
{
	// Définition

public:

	/// Configuration data is json.
	using ConfigData = nlohmann::json;

	// Construction, destruction

public:

	/// Constructor.
	Manager();

	/// Destructor.
	virtual ~Manager();

	// Implementation of IManager

public:

	/// Create a block from its class name.
	///
	/// @param name Name of the block.
	/// @param className Class name of the block.
	///
	/// @return Pointer on the block.
	///
	/// @throw std::runtime_error when another existing block has the same name.
	/// @throw std::runtime_error when no block description registered with this class name.
	IBlock* create(
		const std::string& name,
		const std::string& className) override final;

	/// Find a block from its name.
	///
	/// @param name Name of the block to find.
	///
	/// @return Pointer on the block, nullptr if no block.
	IBlock* find(
		const std::string& name) const override final;

	/// Find a port from its block and class name.
	///
	/// @param block Pointer on the block.
	/// @param name Name of the port to find.
	///
	/// @return Pointer on the port.
	///
	/// @throw std::logic_error when no port registered with this class name
	IPort* find(
		IBlock*            block,
		const std::string& name) const override final;

	// Operations

public:

	/// Prepare the execution.
	///
	/// This method loads the external modules and then create and configure blocks.
	///
	/// @param config The configuration data.
	///
	/// @throw std::exception if unhandled error occurs during the processing.
	void initialize(
		const ConfigData& config);

	/// Start the blocks and wait for terminaison request.
	///
	/// @throw std::exception if unhandled error occurs during the processing.
	void run();

	/// Ask to stop the execution.
	void shutdown();

	// Services

public:

	/// Check the provided name is a valid name for block, route and port.
	///
	/// @param name The name to check.
	///
	/// @return True if the name is valid, false otherwise.
	///
	/// @remarks The name can contains lower case letters, digits and hyphen
	/// and shall start by a letter.
	static bool isValidName(
		const std::string& name);

	// Implementation

private:

	/// Load all possible modules.
	///
	/// Modules localted in the same folder as the executable and modules located
	/// in the additional folders described in the configuration file.
	///
	/// @param config The configuration data.
	void loadModules(
		const ConfigData& config);

	/// Load the external modules located into a given folder.
	///
	/// @param folder Absolute path to the folder to search modules.
	void loadModules(
		const std::filesystem::path& folder);

	/// Create the blocks described into the configuration file.
	///
	/// @param config The configuration data.
	void createBlocks(
		const ConfigData& config);

	/// Create the routes described into the configuration file.
	///
	/// @param config The configuration data.
	void createRoutes(
		const ConfigData& config);

	/// Initialize the blocks when all the blocks and routes has been instancied.
	///
	/// @param config The configuration data.
	///
	void initializeBlocks(
		const ConfigData& config);

	// Private attributes

private:

	/// Registry of blocks description.
	Registry                                           _registry;

	/// Collection of modules loaded dynamically.
	std::list<boost::dll::shared_library>              _modules;

	/// The collection of blocks owned by this manager.
	std::map<std::string, IBlock*>                     _blocks;

	/// The collection of dispatchers that route messages.
	std::map<std::string, std::unique_ptr<Dispatcher>> _dispatchers;

	/// The collection of routes owned by this manager.
	std::list<std::unique_ptr<Route>>                  _routes;

	/// The collection of named routes owned by this manager.
	std::map<std::string, Route*>                      _namedRoutes;

	/// The collection of output ports of the blocks.
	std::list<std::unique_ptr<Port>>                   _ports;
};

} // namespace framework
} // namespace synapse
