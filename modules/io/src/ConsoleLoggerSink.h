///
/// @file ConsoleLoggerSink.h
///
/// Declaration of the ConsoleLoggerSink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <chrono>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

#include <synapse/framework/Sink.h>

namespace synapse {
namespace modules {
namespace io {

///
/// Implement a block that logs incoming data to the console.
///
class ConsoleLoggerSink :
	public synapse::framework::Sink
{
	DECLARE_BLOCK(ConsoleLoggerSink)

	// Définitions

public:

	/// Configuration of the block.
	struct Config
	{
		/// The speedlog pattern.
		std::string pattern;
	};

	// Construction, destruction

private:

	/// Constructor.
	///
	/// @param name Name of the block.
	ConsoleLoggerSink(
		const std::string& name);

	/// Destructor.
	virtual ~ConsoleLoggerSink();

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

	// Private attributes

private:

	/// Configuration data.
	Config _config;
};

} // namespace io
} // namespace modules
} // namespace synapse
