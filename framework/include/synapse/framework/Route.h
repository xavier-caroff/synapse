///
/// @file Route.h
///
/// Declaration of the Route class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <list>

#include "IConsumer.h"

namespace synapse {
namespace framework {

class Port;
class Dispatcher;

///
/// A route is the path to transfer messages from some blocks to
/// destination blocks.
///
class Route
{
	// Construction, destruction

public:

	/// Default constructor.
	///
	/// @param[in] ports The list of sources ports.
	/// @param[in] destinations The list of destinations blocks.
	/// @param[in] dispatcher The distpatcher that will route the messages.
	Route(
		const std::list<Port*>&      ports,
		const std::list<IConsumer*>& destinations,
		Dispatcher*                  dispatcher);

	/// Default destructor.
	virtual ~Route();

	// Accessors

public:

	/// Get the list of sources ports.
	///
	/// @return The list of sources ports.
	const std::list<Port*>&      ports() const { return _ports; }

	/// Get the list of destinations blocks.
	///
	/// @return The list of destinations blocks.
	const std::list<IConsumer*>& destinations() const { return _destinations; }

	// Operation

public:

	/// Dispatch a message to destinations.
	///
	/// @param message The message to dispatch.
	/// @param source The port that issue the message.
	void dispatch(
		const std::shared_ptr<Message>& message,
		const Port&                     source);

	// Private attributes

private:

	/// The list of sources ports.
	std::list<Port*>      _ports;

	/// The list of destinations blocks.
	std::list<IConsumer*> _destinations;

	/// The distpatcher that will route the messages.
	Dispatcher*           _dispatcher;
};

} // namespace framework
} // namespace synapse