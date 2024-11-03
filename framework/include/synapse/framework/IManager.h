///
/// @file IManager.h
///
/// Declaration of the IManager interface.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <string>

#include "IBlock.h"
#include "IPort.h"

namespace synapse {
namespace framework {

///
/// Interface of a block manager.
///
class IManager
{
	// Operations

public:

	/// Create a block from its class name.
	///
	/// @param name Name of the block.
	/// @param className Class name of the block.
	///
	/// @return Pointer on the block.
	///
	/// @throw std::runtime_error when another existing block has the same name.
	/// @throw std::runtime_error when no block description registered with this class name.
	virtual IBlock* create(
		const std::string& name,
		const std::string& className) = 0;

	/// Find a block from its name.
	///
	/// @param name Name of the block to find.
	///
	/// @return Pointer on the block, nullptr if no block.
	virtual IBlock* find(
		const std::string& name) const = 0;

	/// Find a port from its block and class name.
	///
	/// @param block Pointer on the block.
	/// @param name Name of the port to find.
	///
	/// @return Pointer on the port.
	///
	/// @throw std::logic_error when no port registered with this class name
	/// for this block.
	virtual IPort* find(
		IBlock*            block,
		const std::string& name) const = 0;
};

} // namespace framework
} // namespace synapse
