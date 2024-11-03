///
/// @file IConsumer.h
///
/// Declaration of the IConsumer interface.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <memory>

#include "Message.h"

namespace synapse {
namespace framework {

///
/// Interface of a message consumer.
///
class IConsumer
{
	// Operations

public:

	/// Consume a message.
	///
	/// @param message[in] Message to be consumed.
	virtual void consume(
		const std::shared_ptr<Message>& message) = 0;
};

} // namespace framework
} // namespace synapse
