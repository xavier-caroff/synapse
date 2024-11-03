///
/// @file TcpServerSink.h
///
/// Declaration of the TcpServerSink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <cstdint>
#include <string>

#include <synapse/framework/Sink.h>

namespace synapse {
namespace modules {
namespace io {

///
/// Implement a block that read data from a serial port.
///
class TcpServerSink :
	public synapse::framework::Sink
{
	DECLARE_BLOCK(TcpServerSink)

	// Définitions

public:

	/// Configuration of the block.
	struct Config
	{
		/// Host to connect to (logic address as www.google.com or static address as 192.168.64.32).
		std::string host;

		/// TCP port to connect to.
		uint16_t    port;

		/// Delay before reconnecting to the server after a communication loss.
		uint16_t    retryDelay;

		/// Maximum number of bytes to extract in a single read operation.
		size_t      bufferSize;
	};

	/// Maximum size of the read buffer.
	static const size_t MAX_BUFFER_SIZE{ 1024 };

	// Construction, destruction

private:

	/// Constructor.
	///
	/// @param name Name of the block.
	TcpServerSink(
		const std::string& name);

	/// Destructor.
	virtual ~TcpServerSink();

	// Implementation of IBlock

public:

	/// Initialize the block before the execution.
	///
	/// @param[in] configData The configuration data of the block.
	/// @param[in] manager The manager of the block.
	void initialize(
		const ConfigData&             configData,
		synapse::framework::IManager* manager) override;

	// Overload of Sink

protected:

	/// Process a message in the context of the runnable.
	///
	/// @param message[in] Message to be processed.
	void process(
		const std::shared_ptr<synapse::framework::Message>& message) override final;

	// Implementation

private:

	// Private attributes

private:
};

} // namespace io
} // namespace modules
} // namespace synapse
