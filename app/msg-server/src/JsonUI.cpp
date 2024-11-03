///
/// @file JsonUI.cpp
///
/// Implementation of the JsonUI class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <iostream>

#include <nlohmann/json.hpp>

#include "JsonUI.h"
#include "NlohmannEnum.h"

namespace synapse {
namespace app {
namespace msgsrv {

// clang-format off
/// Helper for JSON serialization of enum.
JSON_SERIALIZE_ENUM(synapse::app::msgsrv::IUserInterface::Severity, {
	{  synapse::app::msgsrv::IUserInterface::Severity::debug,   "debug"},
	{   synapse::app::msgsrv::IUserInterface::Severity::info,    "info"},
	{synapse::app::msgsrv::IUserInterface::Severity::warning, "warning"},
	{  synapse::app::msgsrv::IUserInterface::Severity::error,   "error"},
});
// clang-format on

// Output a small message to the user.
void JsonUI::message(
	Severity           severity,
	const std::string& content)
{
	if (severity >= _severityFilter)
	{
		nlohmann::json data;

		data["verb"]   = "message";
		data["params"] = {
			{"severity", severity},
			{ "content",  content}
		};

		std::cout << data << "\r\n";
	}
}

// Configure the level of severity filtering for the message() service.
void JsonUI::setMessageFiltering(
	Severity severity)
{
	_severityFilter = severity;
}

// Display a block of text (one or morelines).
void JsonUI::display(
	const std::string& content)
{
	nlohmann::json data;

	data["verb"]   = "display";
	data["params"] = {
		{"content", content}
	};

	std::cout << data << "\r\n";
}

// Provide version information.
void JsonUI::version(
	unsigned short     major,
	unsigned short     minor,
	unsigned short     micro,
	const std::string& description)
{
	nlohmann::json data;

	data["verb"]   = "version";
	data["params"] = {
		{      "major",       major},
		{      "minor",       minor},
		{      "micro",       micro},
		{"description", description}
	};

	std::cout << data << "\r\n";
}

} // namespace msgsrv
} // namespace app
} // namespace synapse
