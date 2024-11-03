///
/// @file Sink.cpp
///
/// Implementation of the Sink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <fmt/format.h>

#include "synapse/framework/Sink.h"

namespace synapse {
namespace framework {

// Default constructor.
Sink::Sink(
	const std::string& name)
	: BaseBlock(name)
{
}

// Destructor
Sink::~Sink()
{
}

/// Ask the block to prepare to be deleted (terminate all pending operations).
void Sink::shutdown()
{
	_shutdown.store(true);
	_cvMessages.notify_one();
}

// Control function of the runnable.
void Sink::run()
{
	while (_shutdown.load() == false)
	{
		// Wait for an action to be processed.
		{
			std::unique_lock<std::mutex> lock(_mtxMessages);

			_cvMessages.wait(lock, [this] { return _shutdown.load() || !_messages.empty(); });

			// If shutdown was requested, stop the loop.
			if (_shutdown.load())
			{
				break;
			}
		}

		// Process pending messages.
		while (true)
		{
			// Get the next message to process.
			_mtxMessages.lock();
			if (_messages.empty())
			{
				_mtxMessages.unlock();
				break;
			}
			auto message = _messages.front();
			_messages.pop_front();
			_mtxMessages.unlock();

			// Process the message.
			process(message);
		}
	}
}

// Consume a message.
void Sink::consume(
	const std::shared_ptr<Message>& message)
{
	// Enqueue the message.
	{
		std::lock_guard<std::mutex> lock(_mtxMessages);
		_messages.push_back(message);
	}

	// Notify the runnable.
	_cvMessages.notify_one();
}

} // namespace framework
} // namespace synapse
