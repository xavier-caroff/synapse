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

#include "Nmea0183FramerFiber.h"

#define API extern "C" BOOST_SYMBOL_EXPORT

/// Module entry point.
///
/// @param registry The registry to store the blocks published by the
/// module.
API void registerBlocks(
	synapse::framework::Registry& registry)
{
	registry.registerDescription(synapse::modules::marine::Nmea0183FramerFiber::description());
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