///
/// @file Fiber.cpp
///
/// Implementation of the Fiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include "synapse/framework/Fiber.h"

namespace synapse {
namespace framework {

// Default constructor.
Fiber::Fiber(
	const std::string& name)
	: BaseBlock(name)
{
}

// Destructor
Fiber::~Fiber()
{
}

} // namespace framework
} // namespace synapse
