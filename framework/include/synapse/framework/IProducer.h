///
/// @file IProducer.h
///
/// Declaration of the IProducer interface.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <list>
#include <string>

#include "Message.h"

namespace synapse {
namespace framework {

///
/// Interface of a message producer.
///
class IProducer
{
	// Operations

public:

	/// Get the list of output ports.
	virtual std::list<std::string> ports() = 0;
};

} // namespace framework
} // namespace synapse
