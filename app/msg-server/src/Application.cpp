///
/// @file Application.cpp
///
/// Implementation of the Application class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <chrono>
#include <fstream>
#include <iostream>

#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <boost/program_options.hpp>

#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include <synapse/framework/VersionInfo.h>

#include "Application.h"
#include "HumanUI.h"
#include "IUserInterface.h"
#include "JsonUI.h"
#include "Server.h"

namespace synapse {
namespace app {
namespace msgsrv {

Application* Application::_instance = nullptr;

// Constructor.
Application::Application()
{
	// Register the unique instance.
	if (this->_instance == nullptr)
	{
		this->_instance = this;
	}
	else
	{
		throw std::logic_error("only one instance of Application is allowed");
	}

	// Defaut user interface is dedicated to human being.
	_ui.reset(new HumanUI);
}

// Destructor.
Application::~Application()
{
}

// Start the application.
Application::ExitCode Application::execute(
	int   argc,
	char* argv[])
{
	ExitCode    result{ ExitCode::success };
	RunningMode runningMode{ RunningMode::showUsage };

	// Select the format of the CLI output.
	if (result == ExitCode::success)
	{
		try
		{
			updateCliFormat(argc, argv);
		}
		catch (...)
		{
			// Nothing to do
		}
	}

	// Parse command line.
	if (result == ExitCode::success)
	{
		try
		{
			runningMode = parseCommandLine(argc, argv);
		}
		catch (std::exception& error)
		{
			ui()->message(IUserInterface::Severity::error, fmt::format("command line syntax is not valid: {}", error.what()));
			result = ExitCode::syntax;
		}
	}

	// Execute the command.
	if (result == ExitCode::success)
	{
		switch (runningMode)
		{
		case RunningMode::showUsage:
			// Nothing to do, help message already displayed.
			break;
		case RunningMode::showVersion:
			showVersion();
			break;
		case RunningMode::serve:
			result = serve(_serveOptions);
			break;
		default:
			throw std::logic_error("unsupported command");
			break;
		}
	}

	return result;
}

// Update the format of the CLI output depending on the command line argument.
void Application::updateCliFormat(
	int   argc,
	char* argv[])
{
	namespace po = boost::program_options;

	// Description of options.
	po::options_description optionsDescription("Options");
	std::string             cliFormat;

	// clang-format off
	optionsDescription.add_options()
		("cli-format", po::value(&cliFormat)->default_value("human"), "select the format of the CLI output");
	// clang-format on

	po::variables_map       vm;

	store(po::command_line_parser(argc, argv).options(optionsDescription).allow_unregistered().run(), vm);
	notify(vm);

	if (cliFormat == "human")
	{
		// nothing to do.
	}
	else if (cliFormat == "json")
	{
		_ui.reset(new JsonUI);
	}
	else
	{
		ui()->message(IUserInterface::Severity::warning, "unsupported CLI format specified, falling back to the 'human' format");
	}
}

// Parse the command line
Application::RunningMode Application::parseCommandLine(
	int   argc,
	char* argv[])
{
	namespace po = boost::program_options;

	RunningMode             result{ RunningMode::showUsage };
	bool                    found{ false };
	bool                    showUsage{ false };
	std::string             cliFormat;

	// Description of help options.
	po::options_description helpOptionsDescription("Help options");

	// clang-format off
	helpOptionsDescription.add_options()
		("help,h", "produce help message")
		("cli-format", po::value(&cliFormat)->default_value("human"), "select the format of the CLI output ('human' or 'json')");
	// clang-format on

	// Description of version options.
	po::options_description versionOptionsDescription("Version options");

	// clang-format off
	versionOptionsDescription.add_options()
		("version,v", "display version information")
		("cli-format", po::value(&cliFormat)->default_value("human"), "select the format of the CLI output ('human' or 'json')");
	// clang-format on

	// Description of the options for the `serve` command.
	po::options_description serveOptionsDescription("Launch the server");

	// clang-format off
	serveOptionsDescription.add_options()
		("address", po::value(&_serveOptions.address)->default_value("0.0.0.0"), "Address to listen on")
		("port", po::value(&_serveOptions.port)->required(), "TCP port number")
		("delay", po::value(&_serveOptions.delay)->required(), "Delay between to messages (in seconds, eg. 0.1)")
		("file", po::value(&_serveOptions.file)->required(), "Path to the file to be send")
		("block-size", po::value(&_serveOptions.blockSize)->default_value(128), "Size of the blocks")
		("loop", po::value(&_serveOptions.loop)->default_value(true), "Loop when the end of file is reached")
		("cli-format", po::value(&cliFormat)->default_value("human"), "select the format of the CLI output ('human' or 'json')");
	// clang-format on

	// When no argument, show usage.
	if (argc == 1)
	{
		showUsage = true;
		found     = true;
	}

	// Check help command.
	if (!found)
	{
		po::variables_map vm;

		store(po::command_line_parser(argc, argv).options(helpOptionsDescription).allow_unregistered().run(), vm);

		if (vm.count("help") != 0)
		{
			store(po::command_line_parser(argc, argv).options(helpOptionsDescription).run(), vm);
			result    = RunningMode::showUsage;
			showUsage = true;
			found     = true;
		}
	}

	// Check version option.
	if (!found)
	{
		po::variables_map vm;

		store(po::command_line_parser(argc, argv).options(versionOptionsDescription).allow_unregistered().run(), vm);

		if (vm.count("version") != 0)
		{
			store(po::command_line_parser(argc, argv).options(versionOptionsDescription).run(), vm);

			result = RunningMode::showVersion;
			found  = true;
		}
	}

	// Check other options
	if (!found)
	{
		po::variables_map       vm;
		po::options_description cmdlineOptions;

		cmdlineOptions.add(serveOptionsDescription);

		po::positional_options_description positionalOptions;

		positionalOptions.add("file", 1);

		store(po::command_line_parser(argc, argv).options(cmdlineOptions).positional(positionalOptions).run(), vm);
		notify(vm);

		result = RunningMode::serve;
		found  = true;
	}

	// Show usage if requested.
	if (showUsage)
	{
		std::stringstream os;

		os << synapse::framework::getVersion().description << " - " << synapse::framework::getVersion().version
		   << std::endl
		   << std::endl
		   << "Usage: msg-server { -h | -v | {options} <file> }" << std::endl;

		po::options_description cmdlineOptions;

		cmdlineOptions
			.add(helpOptionsDescription)
			.add(versionOptionsDescription)
			.add(serveOptionsDescription);
		os << cmdlineOptions;

		ui()->display(os.str());
	}

	return result;
}

// Display version information.
void Application::showVersion()
{
	ui()->version(
		synapse::framework::getVersion().major,
		synapse::framework::getVersion().minor,
		synapse::framework::getVersion().micro,
		synapse::framework::getVersion().description);
}

// Launch the server.
Application::ExitCode Application::serve(
	const ServeOptions& options)
{
	ExitCode                result = ExitCode::success;
	boost::asio::io_context ioc;

	// Capture SIGINT and SIGTERM to perform a clean shutdown
	boost::asio::signal_set signals(ioc, SIGINT, SIGTERM);
	signals.async_wait(
		[&](boost::system::error_code const&, int) {
			// Stop the `io_context`. This will cause `run()`
			// to return immediately, eventually destroying the
			// `io_context` and all of the sockets in it.
			ioc.stop();
		});

	// Extract configurationfrom command line arguments.
	auto                     duration = std::chrono::duration_cast<boost::asio::steady_timer::duration>(std::chrono::duration<double>(options.delay));
	boost::asio::ip::address address;

	if (result == ExitCode::success)
	{
		boost::asio::ip::tcp::resolver resolver(ioc);
		auto                           results = resolver.resolve(options.address, std::to_string(options.port));

		if (results.empty())
		{
			ui()->message(IUserInterface::Severity::error, "Unable to resolve address");
			result = ExitCode::invalidAddress;
		}
		else
		{
			address = results.begin()->endpoint().address();
		}
	}

	// Run the server.
	if (result == ExitCode::success)
	{
		Server server(
			ioc,
			ui(),
			boost::asio::ip::tcp::endpoint(address, options.port),
			duration,
			options.file,
			options.blockSize,
			options.loop);

		// Start serving.

		ioc.run();

		ui()->message(IUserInterface::Severity::info, "The Server is stopped");
	}

	return result;
}

} // namespace msgsrv
} // namespace app
} // namespace synapse
