///
/// @file Registry.h
///
/// Declaration of the Registry class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <map>
#include <string>
#include <type_traits>

#include "IBlock.h"
#include "IManager.h"

namespace synapse {
namespace framework {

///
/// Registry the description of blocks classes and provide services to
/// instantiate them.
///
class Registry
{
	// Definition

public:

	/// The name of the entry point function.
	static const char constexpr* ENTRY_POINT_FUNCTION{ "registerBlocks" };

	/// Defintion of the signature of the function to create a component.
	using CreateFunction = std::add_pointer<IBlock*(const std::string&)>::type;

	///
	/// Description of a block class.
	///
	struct BlockDescription
	{
		/// Name of the class.
		std::string    _className;

		/// Creation function.
		CreateFunction _create;

		/// Constructor with initialization.
		///
		/// @param className Class name of the component (including namespaces).
		/// @param create Pointer to the function that create the block.
		BlockDescription(
			const std::string& className,
			CreateFunction     create)
			: _className{ className },
			  _create{ create } { }

		/// @cond
		BlockDescription(
			const BlockDescription& other) = default;

		BlockDescription(
			BlockDescription&& other) = default;

		BlockDescription& operator=(
			const BlockDescription& other) = default;

		BlockDescription& operator=(
			BlockDescription&& other) = default;
		/// @endcond
	};

	// Construction, destruction

public:

	/// Default constructor.
	Registry();

	///
	/// Default destructor.
	///
	virtual ~Registry();

	// Operations

public:

	/// Add a new block description to the registry.
	///
	/// @param description Description of the block.
	void registerDescription(
		const BlockDescription& description);

	/// Find the description from a class name.
	///
	/// @param className Class name.
	///
	/// @return Reference to the description.
	///
	/// @throw std::runtime_error When no block description registered with this class name.
	const Registry::BlockDescription& find(
		const std::string& className) const;

	// Private attributes

private:

	/// Collection of the registered block's descriptions.
	std::map<std::string, BlockDescription> _descriptions;
};

} // namespace framework
} // namespace synapse
