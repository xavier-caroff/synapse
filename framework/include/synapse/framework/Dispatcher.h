///
/// @file Dispatcher.h
///
/// Declaration of the Dispatcher class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <atomic>
#include <list>
#include <memory>
#include <mutex>

#include "IRunnable.h"
#include "Message.h"
#include "Port.h"
#include "Route.h"

namespace synapse {
namespace framework {

///
/// A dispatcher is in charge to route messages to the destination blocks.
///
/// When the destination block is a fiber, the processing of the message
/// is performed in the running context of the dispatcher (the dispatcher calls
/// the `consume` method of the destination block).
///
/// When the destination block is a sink, the message is stored in its message
/// queue (using the `consume` method also).
///
class Dispatcher :
	public IRunnable
{
	// Construction, destruction

public:

	/// Default constructor.
	///
	/// @param[in] name The name of the object.
	Dispatcher(
		const std::string& name);

	/// Default destructor.
	virtual ~Dispatcher();

	// Acessors

public:

	/// Get the name of the object.
	///
	/// @return The name of the object.
	const std::string& name() { return _name; }

	// Operations

public:

	/// Dispatch a message to destinations.
	///
	/// @param message The message to dispatch.
	/// @param source The port that issue the message.
	/// @param route The route to dispatch the message.
	void dispatch(
		const std::shared_ptr<Message>& message,
		const Port&                     source,
		const Route&                    route);

	/// Ask the dispatcher to terminate the routing of messages.
	void shutdown();

	// Implementation of IRunnable

public:

	/// Control function of the runnable.
	///
	/// This method is called by the manager in a thread dedicated to the
	/// execution of the runnable.
	void run() override final;

	// Private definitions

private:

	/// A request to dispatch a message.
	struct Request
	{
		/// The message to dispatch.
		std::shared_ptr<Message> message;
		/// The port that issue the message.
		const Port&              source;
		/// The route to dispatch the message.
		const Route&             route;
	};

	// Private attributes

private:

	/// The name of the object.
	std::string             _name;

	/// Indicates that a shutdown has been requested.
	std::atomic<bool>       _shutdown{ false };

	/// The mutex to protect the access to the list of requests.
	std::mutex              _mtxRequests;

	/// The condition variable to detect changes on the list of requests.
	std::condition_variable _cvRequests;

	/// The list of requets.
	std::list<Request>      _requests;
};

} // namespace framework
} // namespace synapse