///
/// @file Server.cpp
///
/// Implementation of the Server class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <chrono>
#include <string>

#include <fmt/format.h>

#include "Application.h"
#include "Server.h"

namespace synapse {
namespace app {
namespace msgsrv {

// Construct a new Server object.
Server::Server(
	boost::asio::io_context&              ioc,
	IUserInterface*                       ui,
	const boost::asio::ip::tcp::endpoint& endpoint,
	boost::asio::steady_timer::duration   delay,
	const std::filesystem::path&          path,
	size_t                                blockSize,
	bool                                  loop)
	: _ioc(ioc),
	  _ui(ui),
	  _acceptor(ioc, endpoint),
	  _delay(delay),
	  _socket(ioc),
	  _timer(ioc),
	  _path(path),
	  _blockSize(blockSize),
	  _loop(loop)
{
	ui->message(
		IUserInterface::Severity::info,
		fmt::format(
			"The server is listening on port {} from [{}]",
			_acceptor.local_endpoint().port(),
			_acceptor.local_endpoint().address().to_string()));

	_stream.open(path, std::ios::in | std::ios::binary);

	if (!_stream.is_open())
	{
		ui->message(
			IUserInterface::Severity::error,
			"Unable to open file");
	}

	_message = new uint8_t[_blockSize];

	doAccept();
}

// Destroy the Server object.
Server::~Server()
{
	delete[] _message;
	_message = nullptr;
}

// Accept a new connection.
void Server::doAccept()
{
	_acceptor.async_accept(
		_socket,
		[this](boost::system::error_code ec) {
			if (!ec)
			{
				_ui->message(
					IUserInterface::Severity::info,
					fmt::format("New connection from {}:{}",
								_socket.remote_endpoint().address().to_string(),
								_socket.remote_endpoint().port()));
				_clients.emplace_back(std::move(_socket));
				if (_clients.size() == 1)
				{
					doWait();
				}
			}
			doAccept();
		});
}

// Wait for the timer before sending a message to the clients.
void Server::doWait()
{
	_timer.expires_after(_delay);
	_timer.async_wait(
		[this](boost::system::error_code ec) {
			if (!ec)
			{
				_stream.read(reinterpret_cast<char*>(_message), _blockSize);
				size_t read = _stream.gcount();
				if (read == 0)
				{
					if (_loop)
					{
						_ui->message(IUserInterface::Severity::info, "End of file reached, reopening the file...");
						_stream.close();
						_stream.open(_path, std::ios::in | std::ios::binary);
						_stream.read(reinterpret_cast<char*>(_message), _blockSize);
						read = _stream.gcount();
					}
					else
					{
						_ui->message(IUserInterface::Severity::info, "End of file reached, shuting down the server");
						_ioc.stop();
						return;
					}
				}

				for (auto& client : _clients)
				{
					auto endPoint = client.remote_endpoint();

					boost::asio::async_write(
						client,
						boost::asio::buffer(_message, read),
						[this, endPoint](boost::system::error_code ec, std::size_t /*length*/) {
							if (ec)
							{
								_ui->message(
									IUserInterface::Severity::error,
									fmt::format(
										"Error sending message to {}:{}: {}",
										endPoint.address().to_string(),
										endPoint.port(),
										ec.message()));
								_clients.remove_if(
									[endPoint](auto& socket) { return socket.remote_endpoint() == endPoint; });
							}
						});
				}

				if (!_clients.empty())
				{
					doWait();
				}
			}
		});
}

} // namespace msgsrv
} // namespace app
} // namespace synapse