///
/// @file Port.h
///
/// <Describe the purpose of this file here>
/// Declaration of the xxx class.
/// Implementation of the xxx class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <list>
#include <string>

#include "IBlock.h"
#include "IPort.h"
#include "Route.h"

namespace synapse {
namespace framework {

///
/// An output port of a block to forward message to attached routes.
///
class Port :
	public IPort
{
	// Definitions

public:

	// Construction, destruction

public:

	/// Constructor.
	///
	/// @param[in] name Name of the port.
	/// @param[in] block Pointer to the associated block.
	Port(
		const std::string& name,
		IBlock*            block);

	/// Destructor.
	virtual ~Port();

	// Accessors

public:

	/// Get the name of the port.
	///
	/// @return The name of the port.
	const std::string& name() const { return _name; }

	/// Get the associated block.
	///
	/// @return Pointer on associated block.
	IBlock*            block() const { return _block; }

	// Implementation of IPort

public:

	/// Forward a message to destinations attached to this port.
	///
	/// @param[in] message The message to dispatch.
	void dispatch(
		const std::shared_ptr<Message>& message) override final;

	// Operations

public:

	/// Attach a route to this port.
	///
	/// @param[in] route The route to attach.
	void attach(
		Route* route);

	// Private attributes

private:

	/// Name of the port.
	std::string       _name;

	/// Pointer to the associated block.
	IBlock*           _block;

	/// The collection of routes attached to this port.
	std::list<Route*> _routes;
};

} // namespace framework
} // namespace synapse