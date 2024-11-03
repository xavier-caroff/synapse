///
/// @file Registry.cpp
///
/// Declaration of the Registry class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <stdexcept>

#include "synapse/framework/Registry.h"

namespace synapse {
namespace framework {

// Constructor.
Registry::Registry()
{
}

// Destructor.
Registry::~Registry()
{
}

// Add a new block description to the registry.
void Registry::registerDescription(
	const BlockDescription& description)
{
	if (_descriptions.find(description._className) == _descriptions.end())
	{
		_descriptions.emplace(description._className, description);
	}
}

// Find the description from a class name.
const Registry::BlockDescription& Registry::find(
	const std::string& className) const
{
	auto itr = _descriptions.find(className);

	if (itr == _descriptions.end())
	{
		throw std::runtime_error("no block description registered with this class name");
	}

	return itr->second;
}

} // namespace framework
} // namespace synapse
