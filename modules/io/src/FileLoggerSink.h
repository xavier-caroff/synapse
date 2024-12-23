///
/// @file FileLoggerSink.h
///
/// Declaration of the FileLoggerSink class.
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
/// Implement a block that logs incoming data to a file.
///
class FileLoggerSink :
	public synapse::framework::Sink
{
	DECLARE_BLOCK(FileLoggerSink)

	// Définitions

public:

	/// Strategy to perform rotation on file.
	enum class RotationStrategy
	{
		/// No rotation, everything is logged in the same file.
		none,
		/// Rotation is performed on time basis.
		time,
		/// Rotation is performed on size basis.
		size,
	};

	/// Configuration of the block.
	struct Config
	{
		/// The folder to store data in.
		std::filesystem::path folder;

		/// The template of the filename (ex. "TEST %Y-%m-%dT%H-%M-%S.log").
		std::string           fileNameTemplate;

		/// The rotation strategy to change periodically the name of the file.
		RotationStrategy      rotationStrategy{ RotationStrategy::none };

		/// The delay to open a new file when time based rotation strategy (minute).
		std::chrono::minutes  rotationDelay{ 0 };

		/// The size to open a new file when size based rotation strategy (byte).
		size_t                rotationSize{ 0 };
	};

	// Construction, destruction

private:

	/// Constructor.
	///
	/// @param name Name of the block.
	FileLoggerSink(
		const std::string& name);

	/// Destructor.
	virtual ~FileLoggerSink();

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

	// Private definition

private:

	/// The clock used to do timestamping.
	/// @todo Replace system clock by UTC clock when available with Clang.
	using clock = std::chrono::system_clock;

	///
	/// Information about the stream where data are currently written.
	///
	struct FileInfo
	{
		/// The full path of the file.
		std::filesystem::path path;

		/// The timestamp of the opening of the file.
		clock::time_point     origin;

		/// The number of bytes logged into the file.
		uint64_t              size;

		/// The file stream.
		std::ofstream         stream;
	};

	// Private attributes

private:

	/// Configuration data.
	Config                  _config;

	/// Information about the file currently logged (can be missing if log was not started).
	std::optional<FileInfo> _fileInfo;
};

} // namespace io
} // namespace modules
} // namespace synapse
