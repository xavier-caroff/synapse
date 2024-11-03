///
/// @file TcpClientSource.cpp
///
/// Implementation of the TcpClientSource class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <string>

#include <spdlog/spdlog.h>

#include "TcpClientSource.h"

namespace synapse {
namespace modules {
namespace io {

IMPLEMENT_BLOCK(TcpClientSource)

const char* TcpClientSource::OUTPUT_PORT_NAME = "default";

/// Convert a json object to a cpp object.
/// @param json JSON object.
/// @param object cpp object.
void        from_json(
		   const nlohmann::json&    json,
		   TcpClientSource::Config& object)
{
	// Mandatory attributes.
	json.at("host").get_to(object.host);
	json.at("port").get_to(object.port);

	// Optional attributes.
	object.retryDelay = json.value<uint16_t>("retryDelay", 2);
	object.bufferSize = json.value<size_t>("bufferSize", 1024);
}

// Constructor.
TcpClientSource::TcpClientSource(
	const std::string& name)
	: Source(name)
{
}

// Destructor.
TcpClientSource::~TcpClientSource()
{
}

// Initialize the block before the execution.
void TcpClientSource::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
	// Call the base class implementation.
	synapse::framework::Source::initialize(configData, manager);

	// Read configuration data.
	_config = readConfig<TcpClientSource::Config>(configData);

	// Find the output port.
	_outputPort = manager->find(this, OUTPUT_PORT_NAME);
}

// Ask the component to prepare to be deleted (terminate all pending operations).
void TcpClientSource::shutdown()
{
	_ioc.stop();
}

// Control function of the runnable.
void TcpClientSource::run()
{
	// Try to resolve the address of the end point.
	boost::asio::ip::tcp::endpoint endPoint;
	{
		boost::asio::io_context        ioc;
		boost::asio::ip::tcp::resolver resolver(ioc);
		auto                           results = resolver.resolve(_config.host, std::to_string(_config.port));

		if (results.empty())
		{
			throw std::runtime_error(fmt::format("unable to resolve address: {}", _config.host));
		}
		endPoint = results.begin()->endpoint();
	}

	// Allocate the buffer to received data in.
	_buffer = std::make_unique<uint8_t[]>(_config.bufferSize);

	// Prepare async objects.
	boost::asio::ip::tcp::socket socket(_ioc);
	boost::asio::steady_timer    retryTimer(_ioc);

	// Prepare the connection to the server.
	doConnect(socket, endPoint, retryTimer);

	// Start async operations.
	_ioc.run();
}

/// Start an async connection.
void TcpClientSource::doConnect(
	boost::asio::ip::tcp::socket&         socket,
	const boost::asio::ip::tcp::endpoint& endPoint,
	boost::asio::steady_timer&            retryTimer)
{
	spdlog::info("Connecting...");
	socket.async_connect(
		endPoint,
		[this, &socket, &endPoint, &retryTimer](boost::system::error_code const& error) {
			if (error)
			{
				spdlog::error("Connection failed: ", error.message());
				doWait(socket, endPoint, retryTimer);
			}
			else
			{
				spdlog::info("Connected");
				doRead(socket, endPoint, retryTimer);
			}
		});
}

// Start to wait for a while before attempting to connect again to the server.
void TcpClientSource::doWait(
	boost::asio::ip::tcp::socket&         socket,
	const boost::asio::ip::tcp::endpoint& endPoint,
	boost::asio::steady_timer&            retryTimer)
{
	spdlog::info("Wait for a while...");
	retryTimer.expires_after(std::chrono::seconds(_config.retryDelay));
	retryTimer.async_wait(
		[this, &socket, &endPoint, &retryTimer](boost::system::error_code error) {
			if (error)
			{
				spdlog::error("Wait failed: ", error.message());
			}
			else
			{
				doConnect(socket, endPoint, retryTimer);
			}
		});
}

// Performs an async read operation.
void TcpClientSource::doRead(
	boost::asio::ip::tcp::socket&         socket,
	const boost::asio::ip::tcp::endpoint& endPoint,
	boost::asio::steady_timer&            retryTimer)
{
	socket.async_read_some(
		boost::asio::buffer(_buffer.get(), _config.bufferSize),
		[this, &socket, &endPoint, &retryTimer](
			boost::system::error_code const& error,
			std::size_t                      bytes) {
			if (error)
			{
				spdlog::error("Read failed: ", error.message());
				socket.close();
				doWait(socket, endPoint, retryTimer);
			}
			else
			{
				// Process the received data.
				auto message = std::make_shared<synapse::framework::Message>(bytes, _buffer.get());

				_outputPort->dispatch(message);

				// Start a new read operation.
				doRead(socket, endPoint, retryTimer);
			}
		});
}

} // namespace io
} // namespace modules
} // namespace synapse
