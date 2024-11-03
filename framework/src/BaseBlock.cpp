///
/// @file BaseBlock.cpp
///
/// Implementation of the BaseBlock class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include "synapse/framework/BaseBlock.h"
#include "synapse/framework/Manager.h"

namespace synapse {
namespace framework {

// Default constructor.
BaseBlock::BaseBlock(
	const std::string& name)
	: _name(name)
{
}

// Destructor
BaseBlock::~BaseBlock()
{
}

// Get the name of the block.
const std::string& BaseBlock::name() const
{
	return _name;
}

// Initialize the block before the execution.
void BaseBlock::initialize(
	const ConfigData& configData,
	IManager*         manager)
{
	(void) configData; // Unused parameter.

	_manager = manager;
}

// Ask the block to delete itself.
void BaseBlock::destroy()
{
	delete this;
}

} // namespace framework
} // namespace synapse
