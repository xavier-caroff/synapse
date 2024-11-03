///
/// @file module.cpp
///
/// Module description.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <boost/dll.hpp>

#include <synapse/framework/Registry.h>

#define API extern "C" BOOST_SYMBOL_EXPORT

/// Module entry point.
///
/// @param registry The registry to store the blocks published by the
/// module.
API void registerBlocks(
	synapse::framework::Registry& registry)
{
	(void) registry; // Unused parameter.
}
