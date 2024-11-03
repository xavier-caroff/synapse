///
/// @file TcpClientSource.h
///
/// Declaration of the TcpClientSource class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <thread>

#include <boost/asio.hpp>

#include <synapse/framework/Port.h>
#include <synapse/framework/Source.h>

namespace synapse {
namespace modules {
namespace io {

///
/// Implement a block that read data from a TCP server.
///
class TcpClientSource :
	public synapse::framework::Source
{
	DECLARE_BLOCK(TcpClientSource)

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
	TcpClientSource(
		const std::string& name);

	/// Destructor.
	virtual ~TcpClientSource();

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
	std::list<std::string> ports() override final { return { OUTPUT_PORT_NAME }; }

	// Implementation of IRunnable

public:

	/// Control function of the runnable.
	///
	/// This method is called by the manager in a thread dedicated to the
	/// execution of the runnable.
	void run() override final;

	// Implementation

private:

	/// Start an async connection.
	///
	/// @param socket The socket to connect on.
	/// @param endPoint The end point to connect on.
	/// @param retryTimer The timer to wait for before retrying to connect.
	void doConnect(
		boost::asio::ip::tcp::socket&         socket,
		const boost::asio::ip::tcp::endpoint& endPoint,
		boost::asio::steady_timer&            retryTimer);

	/// Start to wait for a while before attempting to connect again to the server.
	///
	/// @param socket The socket to connect on.
	/// @param endPoint The end point to connect on.
	/// @param retryTimer The timer to wait for before retrying to connect.
	void doWait(
		boost::asio::ip::tcp::socket&         socket,
		const boost::asio::ip::tcp::endpoint& endPoint,
		boost::asio::steady_timer&            retryTimer);

	/// Performs an async read operation.
	///
	/// @param socket The socket to connect on.
	/// @param endPoint The end point to connect on.
	/// @param retryTimer The timer to wait for before retrying to connect.
	void doRead(
		boost::asio::ip::tcp::socket&         socket,
		const boost::asio::ip::tcp::endpoint& endPoint,
		boost::asio::steady_timer&            retryTimer);

	// Private definitions

private:

	/// Name of the output port.
	static const char* OUTPUT_PORT_NAME;

	// Private attributes

private:

	/// Configuration data.
	Config                     _config;

	/// The buffer to perform readings.
	std::unique_ptr<uint8_t[]> _buffer;

	/// The boost::asio context.
	boost::asio::io_context    _ioc;

	/// The output port.
	synapse::framework::IPort* _outputPort{ nullptr };
};

} // namespace io
} // namespace modules
} // namespace synapse
