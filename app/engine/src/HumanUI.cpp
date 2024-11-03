///
/// @file HumanUI.cpp
///
/// Implementation of the HumanUI class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <iomanip>
#include <iostream>

#include <spdlog/spdlog.h>

#include "HumanUI.h"

namespace synapse {
namespace app {
namespace engine {

// Output a small message to the user.
void HumanUI::message(
	Severity           severity,
	const std::string& content)
{
	switch (severity)
	{
	case IUserInterface::Severity::debug:
		spdlog::debug(content);
		break;
	case IUserInterface::Severity::info:
		spdlog::info(content);
		break;
	case IUserInterface::Severity::warning:
		spdlog::warn(content);
		break;
	case IUserInterface::Severity::error:
		spdlog::error(content);
		break;
	default:
		throw std::logic_error(fmt::format("unsupported level of severity message: {}", static_cast<int>(severity)).c_str());
	}
}

// Configure the level of severity filtering for the message() service.
void HumanUI::setMessageFiltering(
	Severity severity)
{
	auto level = spdlog::level::debug;

	switch (severity)
	{
	case IUserInterface::Severity::debug:
		level = spdlog::level::debug;
		break;
	case IUserInterface::Severity::info:
		level = spdlog::level::info;
		break;
	case IUserInterface::Severity::warning:
		level = spdlog::level::warn;
		break;
	case IUserInterface::Severity::error:
		level = spdlog::level::err;
		break;
	default:
		throw std::logic_error(fmt::format("unsupported level of severity message: {}", static_cast<int>(severity)).c_str());
	}

	spdlog::set_level(level);
}

// Display a block of text (one or morelines).
void HumanUI::display(
	const std::string& content)
{
	std::cout << content;
}

// Provide version information.
void HumanUI::version(
	unsigned short     major,
	unsigned short     minor,
	unsigned short     micro,
	const std::string& description)
{
	(void) description; // Unused parameter.

	std::cout << fmt::format("{} - version {}.{}.{}", description, major, minor, micro) << std::endl;
}

} // namespace engine
} // namespace app
} // namespace synapse
