///
/// @file IUserInterface.h
///
/// Declaration of the IUserInterface class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <string>

namespace synapse {
namespace app {
namespace engine {

///
/// Specify the interface of the User Interface.
///
/// The application use this interface to communicate information
/// to the caller of the application (an human being when the application
/// is launched from the command line or a robot when the application is
/// integrated into a third party application.
///
class IUserInterface
{
	// Public definition

public:

	/// The severity levels associated to a message.
	enum class Severity
	{
		/// Debug.
		debug,
		/// Information.
		info,
		/// Warning.
		warning,
		/// Error,
		error,
	};

	// Operations

public:

	/// Output a small message to the user.
	///
	/// @param severity The severity level associated to the information.
	/// @param content The text content of the information (null terminated).
	virtual void message(
		Severity           severity,
		const std::string& content) = 0;

	/// Configure the level of severity filtering for the message() service.
	///
	/// @param severity The severity to display (lower severity are ignored).
	virtual void setMessageFiltering(
		Severity severity) = 0;

	/// Display a block of text (one or morelines).
	///
	/// @param content The text to be displayed (null terminated).
	virtual void display(
		const std::string& content) = 0;

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
		const std::string& description) = 0;
};

} // namespace engine
} // namespace app
} // namespace synapse
