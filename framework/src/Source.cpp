///
/// @file Source.cpp
///
/// Implementation of the Source class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include "synapse/framework/Source.h"

namespace synapse {
namespace framework {

// Default constructor.
Source::Source(
	const std::string& name)
	: BaseBlock(name)
{
}

// Destructor
Source::~Source()
{
}

} // namespace framework
} // namespace synapse
