///
/// @file Application.cpp
///
/// Implementation of the Application class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>

#include <boost/program_options.hpp>

#include <fmt/format.h>

#include <nlohmann/json.hpp>

#include <synapse/framework/VersionInfo.h>

#include "Application.h"
#include "HumanUI.h"
#include "IUserInterface.h"
#include "JsonUI.h"

namespace synapse {
namespace app {
namespace engine {

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

	// Register signal handlers.
	signal(SIGINT, &Application::onSigTerm);
	signal(SIGTERM, &Application::onSigTerm);
#if defined(SIGQUIT)
	signal(SIGQUIT, &Application::onSigTerm);
#endif // defined(SIGQUIT)
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
		case RunningMode::run:
			result = run(_runOptions);
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

	// Description of the options for the `run` command.
	po::options_description runOptionsDescription("Launch the synapse application");

	// clang-format off
	runOptionsDescription.add_options()
		("config", po::value(&_runOptions.config)->required(), "configuration filename")
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

		cmdlineOptions.add(runOptionsDescription);

		po::positional_options_description positionalOptions;

		positionalOptions.add("config", 1);

		store(po::command_line_parser(argc, argv).options(cmdlineOptions).positional(positionalOptions).run(), vm);
		notify(vm);

		result = RunningMode::run;
		found  = true;
	}

	// Show usage if requested.
	if (showUsage)
	{
		std::stringstream os;

		os << synapse::framework::getVersion().description << " - " << synapse::framework::getVersion().version
		   << std::endl
		   << std::endl
		   << "Usage: synsapse-engine { -h | -v | {options} config }" << std::endl;

		po::options_description cmdlineOptions;

		cmdlineOptions
			.add(helpOptionsDescription)
			.add(versionOptionsDescription)
			.add(runOptionsDescription);
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

// Launch the synapse application.
Application::ExitCode Application::run(
	const RunOptions& options)
{
	ExitCode       result = ExitCode::success;
	nlohmann::json config;

	ui()->message(IUserInterface::Severity::info, "The synapse engine is starting");

	// Load the configuration data from the file.
	if (result == ExitCode::success)
	{
		std::ifstream file;

		file.exceptions(std::ios::failbit);

		try
		{
			file.open(options.config);
			file >> config;
		}
		catch (std::exception& e)
		{
			ui()->message(IUserInterface::Severity::error, fmt::format("Failed to read the config file: {}", e.what()));
			result = ExitCode::configFailure;
		}
	}

	// Initialize the component manager.
	if (result == ExitCode::success)
	{
		try
		{
			_manager.initialize(config);
		}
		catch (std::exception& e)
		{
			ui()->message(IUserInterface::Severity::error, fmt::format("Failed to initialize the manager (see log for details): {}", e.what()));
			result = ExitCode::managerFailure;
		}
	}

	// Run the manager main loop.
	if (result == ExitCode::success)
	{
		try
		{
			_manager.run();
		}
		catch (std::exception& e)
		{
			ui()->message(IUserInterface::Severity::error, fmt::format("Unsupported exception occurs (see log for details): {}", e.what()));
			result = ExitCode::exception;
		}
	}

	ui()->message(IUserInterface::Severity::info, "The synapse engine is stopped");

	return result;
}

// Handler for the SIGINT, SIGTERM and SIGQUIT signals.
void Application::onSigTerm(
	int signum)
{
#if defined(SIGQUIT)
	if (signum == SIGQUIT)
	{
		signum = SIGTERM;
	}
#endif // defined(SIGQUIT)

	if (_instance != nullptr && (signum == SIGINT || signum == SIGTERM))
	{
		_instance->_manager.shutdown();
	}
}

} // namespace engine
} // namespace app
} // namespace synapse
