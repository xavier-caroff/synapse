///
/// @file Application.h
///
/// Declaration of the Application class.
///
/// @author Xavier Caroff <xavier.caroff@free.com>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <exception>
#include <filesystem>
#include <memory>
#include <ostream>

#include <synapse/framework/Manager.h>

#include "IUserInterface.h"

namespace synapse {
namespace app {
namespace engine {

///
/// Application class.
///
class Application
{
	// Definitions

public:

	/// The exit code to return to the operating system.
	enum class ExitCode
	{
		/// No error.
		success = 0,
		/// Unsupported exception caught at the application level.
		exception = 100,
		/// Command line syntax is not valid.
		syntax = 101,
		/// Failed to read the configuration file.
		configFailure = 102,
		/// Failed to initialize the manager.
		managerFailure = 102,
	};

	/// Running modes of the application
	enum class RunningMode
	{
		/// Show command line usage.
		showUsage,
		/// Show version.
		showVersion,
		/// Launch the synapse application.
		run,
	};

	/// The different format to output information.
	enum class CliFormat
	{
		/// Human readable.
		human,
	};

	/// Options for the `run` command.
	struct RunOptions
	{
		/// Configuration filename.
		std::filesystem::path config;
	};

	// Construction, destruction

public:

	// Constructor.
	Application();

	// Destructor.
	virtual ~Application();

	// Operations

public:

	/// Start the application.
	///
	/// @param argc Number of arguments on the command line.
	/// @param argv Table of arguments on the command line.
	///
	/// @return Exit code to pass to the operating system (0 is success).
	ExitCode execute(
		int   argc,
		char* argv[]);

	// Accessors

private:

	/// Provide access to the user interface.
	IUserInterface* ui() const { return _ui.get(); }

	// Implementation

private:

	/// Update the format of the CLI output depending on the command line argument.
	///
	/// @param argc Number of arguments on the command line.
	/// @param argv Table of arguments on the command line.
	///
	/// @remarks When the format is not supported the Human format is used.
	void updateCliFormat(
		int   argc,
		char* argv[]);

	/// Parse the command line
	///
	/// @param argc Number of arguments on the command line.
	/// @param argv Table of arguments on the command line.
	///
	/// @return Running mode.
	///
	/// @throw std::exception If the command line is not valid.
	RunningMode parseCommandLine(
		int   argc,
		char* argv[]);

	/// Display version information.
	void     showVersion();

	/// Launch the synapse application.
	///
	/// @param options Options passed from the command line.
	ExitCode run(
		const RunOptions& options);

	/// Handler for the SIGINT, SIGTERM and SIGQUIT signals.
	///
	/// @param signum The received signal.
	static void onSigTerm(
		int signum);

	// Implementation

private:

	// Command line arguments

private:

	/// Options for the `run` command.
	RunOptions _runOptions;

	// Private attributes

private:

	/// Pointer to the unique instance.
	static Application*             _instance;

	/// The implementation of the user interface.
	std::unique_ptr<IUserInterface> _ui;

	/// The synapse manager.
	synapse::framework::Manager     _manager;
};

} // namespace engine
} // namespace app
} // namespace synapse
