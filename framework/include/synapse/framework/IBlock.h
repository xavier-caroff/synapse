///
/// @file IBlock.h
///
/// Declaration of the IBlock interface.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <string>

#include <nlohmann/json.hpp>

#include "Message.h"

namespace synapse {
namespace framework {

class IManager;

///
/// Base interface of blocks.
///
class IBlock
{
	// Définitions

public:

	/// Configuration data is json.
	using ConfigData = nlohmann::json;

	// Enquiries

public:

	/// Get the name of the block.
	///
	/// @return The name of the block.
	virtual const std::string& name() const = 0;

	// Operations

public:

	/// Initialize the block before the execution.
	///
	/// @param[in] configData The configuration data of the block.
	/// @param[in] manager The manager of the block.
	virtual void initialize(
		const ConfigData& configData,
		IManager*         manager) = 0;

	/// Ask the block to prepare to be deleted (terminate all pending operations).
	virtual void shutdown() = 0;

	/// Ask the block to delete itself.
	virtual void destroy() = 0;
};

} // namespace framework
} // namespace synapse
