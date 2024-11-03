///
/// @file JsonUI.h
///
/// Declaration of the JsonUI class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <string>

#include "IUserInterface.h"

namespace synapse {
namespace app {
namespace msgsrv {

///
/// The implementation of the user interface specialized for interaction
/// with robot in JSON format.
///
class JsonUI :
	public IUserInterface
{
	// Operations

public:

	/// Output a small message to the user.
	///
	/// @param severity The severity level associated to the information.
	/// @param content The text content of the information (null terminated).
	void message(
		Severity           severity,
		const std::string& content) override;

	/// Configure the level of severity filtering for the message() service.
	///
	/// @param severity The severity to display (lower severity are ignored).
	virtual void setMessageFiltering(
		Severity severity) override;

	/// Display a block of text (one or morelines).
	///
	/// @param content The text to be displayed (null terminated).
	virtual void display(
		const std::string& content) override;

	/// Provide version information.
	///
	/// @param major Major version number
	/// @param minor Minor version number
	/// @param micro Micro version number
	/// @param description Description of the component.
	virtual void version(
		unsigned short     major,
		unsigned short     minor,
		unsigned short     micro,
		const std::string& description) override;

	// Private attribute.

private:

	/// The severity filter value (lower severity are ignored).
	IUserInterface::Severity _severityFilter{ IUserInterface::Severity::debug };
};

} // namespace msgsrv
} // namespace app
} // namespace synapse
