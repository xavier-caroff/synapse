///
/// @file Port.cpp
///
/// Implementation of the Port class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include "synapse/framework/Port.h"

namespace synapse {
namespace framework {

// Constructor.
Port::Port(
	const std::string& name,
	IBlock*            block)
	: _name(name),
	  _block(block)
{
}

// Destructor.
Port::~Port()
{
}

// Forward a message to destinations attached to this port.
void Port::dispatch(
	const std::shared_ptr<Message>& message)
{
	for (auto& route : _routes)
	{
		route->dispatch(message, *this);
	}
}

// Attach a route to this port.
void Port::attach(
	Route* route)
{
	// Register a route only one time.
	if (std::find(_routes.cbegin(), _routes.cend(), route) == _routes.cend())
	{
		_routes.push_back(route);
	}
}

} // namespace framework
} // namespace synapse