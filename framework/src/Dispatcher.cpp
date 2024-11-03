///
/// @file Dispatcher.cpp
///
/// Implementation of the Dispatcher class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include "synapse/framework/Dispatcher.h"

namespace synapse {
namespace framework {

// Default constructor.
Dispatcher::Dispatcher(
	const std::string& name)
	: _name(name)
{
}

// Default destructor.
Dispatcher::~Dispatcher()
{
}

// Dispatch a message to destinations.
void Dispatcher::dispatch(
	const std::shared_ptr<Message>& message,
	const Port&                     source,
	const Route&                    route)
{
	// Enqueue the request.
	{
		std::lock_guard<std::mutex> lock(_mtxRequests);
		_requests.push_back({ message, source, route });
	}

	// Notify the runnable.
	_cvRequests.notify_one();
}

// Ask the dispatcher to terminate the routing of messages.
void Dispatcher::shutdown()
{
	_shutdown.store(true);
	_cvRequests.notify_one();
}

// Control function of the runnable.
void Dispatcher::run()
{
	while (_shutdown.load() == false)
	{
		// Wait for an action to be processed.
		{
			std::unique_lock<std::mutex> lock(_mtxRequests);

			_cvRequests.wait(lock, [this] { return _shutdown.load() || !_requests.empty(); });

			// If shutdown was requested, stop the loop.
			if (_shutdown.load())
			{
				break;
			}
		}

		// Process pending messages.
		while (true)
		{
			// Get the next request to process.
			_mtxRequests.lock();
			if (_requests.empty())
			{
				_mtxRequests.unlock();
				break;
			}
			auto request = _requests.front();
			_requests.pop_front();
			_mtxRequests.unlock();

			// Process the request.
			for (auto& current : request.route.destinations())
			{
				current->consume(request.message);
			}
		}
	}
}

} // namespace framework
} // namespace synapse