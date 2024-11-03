///
/// @file Source.h
///
/// Declaration of the Source class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include "BaseBlock.h"
#include "IProducer.h"
#include "IRunnable.h"

namespace synapse {
namespace framework {

///
/// A source is a block that generate messages that can be routed to
/// other blocks.
///
class Source :
	public BaseBlock,
	public IProducer,
	public IRunnable
{
	// Construction, destruction

public:

	/// Default constructor.
	///
	/// @param[in] name The name of the block.
	Source(
		const std::string& name);

	/// Default destructor.
	virtual ~Source();
};

} // namespace framework
} // namespace synapse