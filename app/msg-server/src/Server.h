///
/// @file Server.h
///
/// Declaration of the Server class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <chrono>
#include <filesystem>
#include <fstream>
#include <list>
#include <string>

#include <boost/asio.hpp>

#include "IUserInterface.h"

namespace synapse {
namespace app {
namespace msgsrv {

class Server
{
	// Construction, destruction

public:

	/// Construct a new Server object.
	///
	/// @param ioc The boost io context.
	/// @param ui Pointer to the user interface.
	/// @param endpoint TCP endpoint of the server.
	/// @param delay between two messages
	/// @param path The path of the file to send.
	/// @param blockSize Size of the blocks.
	/// @param loop Loop when the end of file is reached.
	Server(
		boost::asio::io_context&              ioc,
		IUserInterface*                       ui,
		const boost::asio::ip::tcp::endpoint& endpoint,
		boost::asio::steady_timer::duration   delay,
		const std::filesystem::path&          path,
		size_t                                blockSize,
		bool                                  loop);

	/// Destroy the Server object.
	virtual ~Server();

	// Implementation

private:

	/// Accept a new connection.
	void doAccept();

	/// Wait for the timer before sending a message to the clients.
	void doWait();

	// Private attributes

private:

	boost::asio::io_context&                _ioc;
	IUserInterface*                         _ui;
	boost::asio::ip::tcp::acceptor          _acceptor;
	boost::asio::ip::tcp::socket            _socket;
	std::list<boost::asio::ip::tcp::socket> _clients;
	boost::asio::steady_timer               _timer;
	uint8_t*                                _message{ nullptr };
	boost::asio::steady_timer::duration     _delay;
	std::ifstream                           _stream;
	std::filesystem::path                   _path;
	size_t                                  _blockSize;
	bool                                    _loop;
};

} // namespace msgsrv
} // namespace app
} // namespace synapse