///
/// @file Sink.h
///
/// Declaration of the Sink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <atomic>
#include <list>
#include <memory>
#include <mutex>

#include "BaseBlock.h"
#include "IConsumer.h"
#include "IRunnable.h"

namespace synapse {
namespace framework {

///
/// A sink is a block that process messages received from
/// other blocks.
///
class Sink :
	public BaseBlock,
	public IConsumer,
	public IRunnable
{
	// Construction, destruction

public:

	/// Default constructor.
	///
	/// @param[in] name The name of the block.
	Sink(
		const std::string& name);

	/// Default destructor.
	virtual ~Sink();

	// Implementation of IBlock

public:

	/// Ask the block to prepare to be deleted (terminate all pending operations).
	void shutdown() override;

	// Implementation of IRunnable

public:

	/// Control function of the runnable.
	///
	/// This method is called by the manager in a thread dedicated to the
	/// execution of the runnable.
	void run() override final;

	// Implementation of IConsumer

public:

	/// Consume a message.
	///
	/// @param message[in] Message to be consumed.
	void consume(
		const std::shared_ptr<Message>& message) override final;

	// Implementation

protected:

	/// Process a message in the context of the runnable.
	///
	/// @param message[in] Message to be processed.
	virtual void process(
		const std::shared_ptr<Message>& message) = 0;

	// Private attributes.

private:

	/// Indicates that a shutdown has been requested.
	std::atomic<bool>                   _shutdown{ false };

	/// The mutex to protect the access to the list of messages.
	std::mutex                          _mtxMessages;

	/// The condition variable to detect changes on the list of messages.
	std::condition_variable             _cvMessages;

	/// The list of messages.
	std::list<std::shared_ptr<Message>> _messages;
};

} // namespace framework
} // namespace synapse