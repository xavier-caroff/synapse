///
/// @file IPort.h
///
/// Declaration of the IPort interface.
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
/// Interface for a port.
///
class IPort
{
	// Operations

public:

	/// Forward a message to destinations attached to this port.
	///
	/// @param[in] message The message to dispatch.
	virtual void dispatch(
		const std::shared_ptr<Message>& message) = 0;
};

} // namespace framework
} // namespace synapse