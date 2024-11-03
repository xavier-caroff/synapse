///
/// @file IRunnable.h
///
/// Declaration of the IRunnable interface.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

namespace synapse {
namespace framework {

///
/// Base interface for a block that has its own thread to run.
///
/// The blocks that implements this interface are started by the
/// manager after the initialization phase.
///
class IRunnable
{
	// Operations

public:

	/// Control function of the runnable.
	///
	/// This method is called by the manager in a thread dedicated to the
	/// execution of the runnable.
	virtual void run() = 0;
};

} // namespace framework
} // namespace synapse
