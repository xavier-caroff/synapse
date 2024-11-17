///
/// @file module.cpp
///
/// Module description.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <boost/dll.hpp>

#include <spdlog/spdlog.h>

#include <synapse/framework/Registry.h>

#include "ConsoleLoggerSink.h"
#include "FileLoggerSink.h"
#include "FramerFiber.h"
#include "SerialSource.h"
#include "TcpClientSource.h"
#include "TcpServerSink.h"

#define API extern "C" BOOST_SYMBOL_EXPORT

/// Module entry point.
///
/// @param registry The registry to store the blocks published by the
/// module.
API void registerBlocks(
	synapse::framework::Registry& registry)
{
	registry.registerDescription(synapse::modules::io::ConsoleLoggerSink::description());
	registry.registerDescription(synapse::modules::io::FileLoggerSink::description());
	registry.registerDescription(synapse::modules::io::FramerFiber::description());
	registry.registerDescription(synapse::modules::io::SerialSource::description());
	registry.registerDescription(synapse::modules::io::TcpClientSource::description());
	registry.registerDescription(synapse::modules::io::TcpServerSink::description());
}

/// Perpare the module's logger.
///
/// @param level The logger to configure.
API void prepareLogger(
	const std::string&        pattern,
	spdlog::level::level_enum level)
{
	// Set the Speedlog pattern to output the severity level and the text an
	// color the severity level.
	spdlog::set_pattern(pattern);
	spdlog::set_level(level);
}