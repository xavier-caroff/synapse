///
/// @file FileLoggerSink.cpp
///
/// Implementation of the FileLoggerSink class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <string>

#include <spdlog/spdlog.h>

#include "FileLoggerSink.h"

namespace synapse {
namespace modules {
namespace io {

IMPLEMENT_BLOCK(FileLoggerSink)

// clang-format off
NLOHMANN_JSON_SERIALIZE_ENUM( FileLoggerSink::RotationStrategy, {
	{ FileLoggerSink::RotationStrategy::none,	"none" },
	{ FileLoggerSink::RotationStrategy::time,	"time" },
	{ FileLoggerSink::RotationStrategy::size,	"size" },
})
// clang-format on

/// Convert a json object to a cpp object.
/// @param json JSON object.
/// @param object cpp object.
void from_json(
	const nlohmann::json&   json,
	FileLoggerSink::Config& object)
{
	object.folder           = json.at("folder").get<std::string>();
	object.fileNameTemplate = json.at("fileNameTemplate").get<std::string>();
	object.rotationStrategy = FileLoggerSink::RotationStrategy::none;
	object.rotationDelay    = std::chrono::minutes{ 0 };
	object.rotationSize     = 0;

	if (json.find("rotation") != json.end())
	{
		object.rotationStrategy = json.at("rotation").at("strategy").get<FileLoggerSink::RotationStrategy>();

		switch (object.rotationStrategy)
		{
		default:
		case FileLoggerSink::RotationStrategy::none:
			// Nothing to do
			break;
		case FileLoggerSink::RotationStrategy::time:
			object.rotationDelay = std::chrono::minutes{ json.at("rotation").at("delay") };
			break;
		case FileLoggerSink::RotationStrategy::size:
			object.rotationSize = json.at("rotation").at("size");
			break;
		}
	}
}

// Constructor.
FileLoggerSink::FileLoggerSink(
	const std::string& name)
	: Sink(name)
{
}

// Destructor.
FileLoggerSink::~FileLoggerSink()
{
	shutdown();
}

// Initialize the block before the execution.
void FileLoggerSink::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
	// Call the base class implementation.
	synapse::framework::Sink::initialize(configData, manager);

	// Read configuration data.
	_config = readConfig<FileLoggerSink::Config>(configData);
}

// Process a message in the context of the runnable.
void FileLoggerSink::process(
	const std::shared_ptr<synapse::framework::Message>& message)
{
	// Perform rotation if needed.
	bool rotate =
		(_fileInfo &&
		 _config.rotationStrategy == FileLoggerSink::RotationStrategy::time &&
		 clock::now() >= _fileInfo->origin + _config.rotationDelay) ||
		(_fileInfo &&
		 _config.rotationStrategy == FileLoggerSink::RotationStrategy::size &&
		 _fileInfo->size >= _config.rotationSize);

	if (rotate)
	{
		spdlog::warn("FileLoggerSink::process() - rotate");
		if (_fileInfo->stream.is_open())
		{
			_fileInfo->stream.close();
		}

		_fileInfo.reset();
	}

	// Prepare file info if not intialized.
	if (!_fileInfo)
	{
		auto now        = clock::now();
		auto nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);

		_fileInfo = {
			_config.folder / std::vformat(_config.fileNameTemplate, std::make_format_args(nowSeconds)),
			now,
			0,
			std::ofstream{}
		};

		_fileInfo->stream.open(_fileInfo->path, std::ios_base::out | std::ios_base::binary);
		if (!_fileInfo->stream.is_open())
		{
			spdlog::error("{}: failed to open file {}", name(), _fileInfo->path.string());
		}
	}

	// Write the message to the file.
	if (message->size() > 0)
	{
		_fileInfo->stream.write(reinterpret_cast<const char*>(message->payload()), message->size());
		_fileInfo->stream.flush();
		_fileInfo->size += message->size();
	}
}

} // namespace io
} // namespace modules
} // namespace synapse
