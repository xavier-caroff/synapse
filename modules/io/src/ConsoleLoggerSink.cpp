///
/// @file ConsoleLoggerSink.cpp
///
/// Implementation of the ConsoleLoggerSink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <iostream>
#include <string>

#include <spdlog/spdlog.h>

#include "ConsoleLoggerSink.h"

namespace synapse {
namespace modules {
namespace io {

IMPLEMENT_BLOCK(ConsoleLoggerSink)

/// Convert a json object to a cpp object.
/// @param json JSON object.
/// @param object cpp object.
void from_json(
	const nlohmann::json&      json,
	ConsoleLoggerSink::Config& object)
{
	object.pattern = json.at("pattern").get<std::string>();
}

// Constructor.
ConsoleLoggerSink::ConsoleLoggerSink(
	const std::string& name)
	: Sink(name)
{
}

// Destructor.
ConsoleLoggerSink::~ConsoleLoggerSink()
{
	shutdown();
}

// Initialize the block before the execution.
void ConsoleLoggerSink::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
	// Call the base class implementation.
	synapse::framework::Sink::initialize(configData, manager);

	// Read configuration data.
	_config = readConfig<ConsoleLoggerSink::Config>(configData);
}

// Process a message in the context of the runnable.
void ConsoleLoggerSink::process(
	const std::shared_ptr<synapse::framework::Message>& message)
{
	std::cout << _config.pattern << " | " << std::string_view(reinterpret_cast<const char*>(message->payload()), message->size()) << std::endl;
}

} // namespace io
} // namespace modules
} // namespace synapse
