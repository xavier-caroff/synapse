///
/// @file VersionInfo.h
///
/// Declaration of the VersionInfo class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <string>

namespace synapse {
namespace framework {

///
/// Version information of the library.
///
struct VersionInfo
{
	/// Major version number
	unsigned short major;
	/// Minor version number
	unsigned short minor;
	/// Micro version number
	unsigned short micro;

	/// Version as a string.
	std::string    version;

	/// Description of the library.
	std::string    description;
};

/// Get access to the version information of the library.
///
/// @return The version information of the library.
const VersionInfo& getVersion();

} // namespace framework
} // namespace synapse
