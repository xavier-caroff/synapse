///
/// @file SerialSource.h
///
/// Declaration of the SerialSource class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <cstdint>
#include <string>

#include <synapse/framework/Port.h>
#include <synapse/framework/Source.h>

namespace synapse {
namespace modules {
namespace io {

///
/// Implement a block that read data from a serial port.
///
class SerialSource :
	public synapse::framework::Source
{
	DECLARE_BLOCK(SerialSource)

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
	SerialSource(
		const std::string& name);

	/// Destructor.
	virtual ~SerialSource();

	// Implementation of IBlock

public:

	/// Initialize the block before the execution.
	///
	/// @param[in] configData The configuration data of the block.
	/// @param[in] manager The manager of the block.
	void initialize(
		const ConfigData&             configData,
		synapse::framework::IManager* manager) override;

	/// Ask the block to prepare to be deleted (terminate all pending operations).
	void shutdown() override final;

	// Implementation of IProducer

public:

	/// Get the list of output ports.
	std::list<std::string> ports() override final { return { "default" }; }

	// Implementation of IRunnable

public:

	/// Control function of the runnable.
	///
	/// This method is called by the manager in a thread dedicated to the
	/// execution of the runnable.
	void run() override final;

	// Implementation

private:

	// Private attributes

private:
};

} // namespace io
} // namespace modules
} // namespace synapse
