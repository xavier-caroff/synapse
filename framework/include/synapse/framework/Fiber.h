///
/// @file Fiber.h
///
/// Declaration of the Fiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include "BaseBlock.h"
#include "IConsumer.h"
#include "IProducer.h"

namespace synapse {
namespace framework {

///
/// A fiber is a block that process messages an forward result to
/// other blocks.
///
class Fiber :
	public BaseBlock,
	public IConsumer,
	public IProducer
{
	// Construction, destruction

public:

	/// Default constructor.
	///
	/// @param[in] name The name of the block.
	Fiber(
		const std::string& name);

	/// Default destructor.
	virtual ~Fiber();
};

} // namespace framework
} // namespace synapse