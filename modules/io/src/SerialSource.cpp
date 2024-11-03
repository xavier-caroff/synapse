///
/// @file SerialSource.cpp
///
/// Implementation of the SerialSource class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <string>

#include "SerialSource.h"

namespace synapse {
namespace modules {
namespace io {

IMPLEMENT_BLOCK(SerialSource)

// Constructor.
SerialSource::SerialSource(
	const std::string& name)
	: Source(name)
{
}

// Destructor.
SerialSource::~SerialSource()
{
	shutdown();
}

// Initialize the block before the execution.
void SerialSource::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
}

// Ask the component to prepare to be deleted (terminate all pending operations).
void SerialSource::shutdown()
{
}

// Control function of the runnable.
void SerialSource::run()
{
}

} // namespace io
} // namespace modules
} // namespace synapse
