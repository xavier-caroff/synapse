///
/// @file HumanUI.h
///
/// Declaration of the HumanUI class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include "IUserInterface.h"

namespace synapse {
namespace app {
namespace engine {

///
/// The implementation of the user interface specialized for interaction
/// with human being.
///
class HumanUI :
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
};

} // namespace engine
} // namespace app
} // namespace synapse
