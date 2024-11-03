///
/// @file main.cpp
///
/// Entry point of the application.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <iostream>

#include <spdlog/spdlog.h>

#include "Application.h"

/// Entry point of the executable
///
/// @param argc Number of arguments on the command line.
/// @param argv Table of arguments on the command line.
///
/// @return Exit code to pass to the operating system (0 is success).
int main(
	int   argc,
	char* argv[])
{
	synapse::app::engine::Application           app;
	synapse::app::engine::Application::ExitCode result;

	// Set the Speedlog pattern to output the severity level and the text an
	// color the severity level.
	spdlog::set_pattern("%^%l%$: %v");

	// Exécute the application.
	try
	{
		result = app.execute(argc, argv);
	}
	catch (const std::exception& e)
	{
		std::cerr << "unsupported error:" << e.what() << std::endl;
		result = synapse::app::engine::Application::ExitCode::exception;
	}

	return static_cast<int>(result);
}