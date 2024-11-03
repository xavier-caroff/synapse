///
/// @file Route.cpp
///
/// Implementation of the Route class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include "synapse/framework/Route.h"
#include "synapse/framework/Dispatcher.h"
#include "synapse/framework/Port.h"

namespace synapse {
namespace framework {

// Default constructor.
Route::Route(
	const std::list<Port*>&      ports,
	const std::list<IConsumer*>& destinations,
	Dispatcher*                  dispatcher)
	: _ports(ports),
	  _destinations(destinations),
	  _dispatcher(dispatcher)
{
}

// Default destructor.
Route::~Route()
{
}

// Dispatch a message to destinations.
void Route::dispatch(
	const std::shared_ptr<Message>& message,
	const Port&                     source)
{
	_dispatcher->dispatch(message, source, *this);
}

} // namespace framework
} // namespace synapse