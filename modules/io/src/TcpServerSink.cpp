///
/// @file TcpServerSink.cpp
///
/// Implementation of the TcpServerSink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <string>

#include <spdlog/spdlog.h>

#include "TcpServerSink.h"

namespace synapse {
namespace modules {
namespace io {

IMPLEMENT_BLOCK(TcpServerSink)

// Constructor.
TcpServerSink::TcpServerSink(
	const std::string& name)
	: Sink(name)
{
}

// Destructor.
TcpServerSink::~TcpServerSink()
{
	shutdown();
}

// Initialize the block before the execution.
void TcpServerSink::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
}

// Process a message in the context of the runnable.
void TcpServerSink::process(
	const std::shared_ptr<synapse::framework::Message>& message)
{
}

} // namespace io
} // namespace modules
} // namespace synapse
