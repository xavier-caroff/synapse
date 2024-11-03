///
/// @file Nmea0183FramerFiber.cpp
///
/// Implementation of the Nmea0183FramerFiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <regex>
#include <string>

#include <fmt/format.h>

#include <spdlog/spdlog.h>

#include "Nmea0183FramerFiber.h"

namespace synapse {
namespace modules {
namespace marine {

IMPLEMENT_BLOCK(Nmea0183FramerFiber)

const char* Nmea0183FramerFiber::OUTPUT_PORT_NAME = "default";

/// Conversion of a json object to a Framer::Config object.
///
/// @param json The json representation of the object's content.
/// @param object The object to initialize.
static void from_json(
	const nlohmann::json&        json,
	Nmea0183FramerFiber::Config& object)
{
	object.bufferSize = json.value("bufferSize", Nmea0183FramerFiber::DEFAULT_BUFFER_SIZE);
}

// Constructor.
Nmea0183FramerFiber::Nmea0183FramerFiber(
	const std::string& name)
	: Fiber(name)
{
}

// Destructor.
Nmea0183FramerFiber::~Nmea0183FramerFiber()
{
	shutdown();

	if (_buffer)
	{
		delete[] _buffer;
		_buffer = nullptr;
	}
}

// Initialize the block before the execution.
void Nmea0183FramerFiber::initialize(
	const ConfigData&             configData,
	synapse::framework::IManager* manager)
{
	// Get configuration data.
	_config = readConfig<Nmea0183FramerFiber::Config>(configData);

	// Find the output port.
	_outputPort = manager->find(this, OUTPUT_PORT_NAME);

	// Allocate the buffer.
	_buffer     = new uint8_t[_config.bufferSize];
	_bufferSize = _config.bufferSize;
}

// Ask the component to prepare to be deleted (terminate all pending operations).
void Nmea0183FramerFiber::shutdown()
{
}

// Consume a message.
void Nmea0183FramerFiber::consume(
	const std::shared_ptr<synapse::framework::Message>& message)
{
	// Nothing to do if the content of the message is empty.
	bool goOn = message->size() > 0;

	// If the buffer is empty, try to extract frames from the message
	// and store the rest in the buffer.
	if (goOn && _bufferCount == 0)
	{
		auto     begin = message->payload();
		auto     end   = message->payload() + message->size();
		size_t   length{ 0 };
		uint8_t* start{ nullptr };

		while (begin < end)
		{
			auto found = findFrame(begin, end, length, start);

			if (found)
			{
				// Create a message and forward it.
				_outputPort->dispatch(std::make_shared<synapse::framework::Message>(length, found));

				// Warn if some bytes where skipped.
				if (found != begin)
				{
					spdlog::warn("{}: {} bytes skipped.", name(), found - begin);
				}
				// Adjust the new begin position for the search.
				begin = found + length;
			}
			else
			{
				// If a start sequence is found, keep the rest in the buffer.
				if (start != nullptr)
				{
					if (start != begin)
					{
						spdlog::warn("{}: {} bytes skipped.", name(), start - begin);
					}
					// If the buffer is too small to copy all the message the first part of
					// the message is discarded.
					if (static_cast<size_t>(end - start) > _bufferSize)
					{
						auto lost = (end - start) - _bufferSize;
						start += lost;
						spdlog::error("{}: {} bytes discarded since the buffer is too small.", name(), lost);
					}
					std::memcpy(_buffer, start, end - start);
					_bufferCount = end - start;
				}
				// No start sequence found.
				else
				{
					// Number of bytes left in the message.
					size_t left = end - begin;
					spdlog::warn("{}: {} bytes skipped.", name(), left);
					_bufferCount = 0;
				}
				break;
			}
		}
		goOn = false;
	}
	// Otherwise if the buffer is not empty, add the content of the message.
	else if (goOn)
	{
		// If the space that left in the buffer is not large enough to add the message, the
		// last bytes of buffer + message are kept.
		if (_bufferCount + message->size() > _bufferSize)
		{
			// Warn the user if message size is too large compared to the size of the buffer.
			if (message->size() > _bufferSize)
			{
				spdlog::error("{}: the size of the buffer ({} bytes) is too small compared to the size of the message ({} bytes).", name(), _bufferSize, message->size());
			}

			// When the size of the message is greater or equal to the size of the buffer,
			// the last part of the message is copied in the buffer. The current content of
			// the buffer is lost.
			if (message->size() >= _bufferSize)
			{
				auto skipped = _bufferCount + message->size() - _bufferSize;
				spdlog::warn("{}: {} bytes skipped.", name(), skipped);
				auto size  = _bufferSize;
				auto start = message->payload() + message->size() - size;
				std::memcpy(_buffer, start, size);
				_bufferCount = size;
			}
			// When the size of the message is smaller than the size of the buffer,
			// the content of the message is copied at the end of the buffer and the
			// last part of the bytes currently in the buffer are kept.
			else
			{
				auto left = _bufferSize - message->size();
				auto lost = _bufferCount - left;
				spdlog::warn("{}: {} bytes skipped.", name(), lost);
				std::memmove(_buffer, _buffer + lost, left);
				std::memcpy(_buffer + left, message->payload(), message->size());
				_bufferCount = left + message->size();
			}
		}
		// Otherwise simply add the content of the message.
		else
		{
			std::memcpy(_buffer + _bufferCount, message->payload(), message->size());
			_bufferCount += message->size();
		}
	}

	// Try to extract frames from the buffer (if there is at least enough bytes).
	if (goOn &&
		_bufferCount > 0 &&
		_bufferCount >= 1 + 2) // 1 = "$", 2 = "\r\n"
	{
		auto     begin = _buffer;
		auto     end   = _buffer + _bufferCount;
		size_t   length{ 0 };
		uint8_t* start{ nullptr };

		while (begin < end)
		{
			auto found = findFrame(begin, end, length, start);

			if (found)
			{
				// Create a message and forward it.
				_outputPort->dispatch(std::make_shared<synapse::framework::Message>(length, found));

				// Warn if some bytes where skipped.
				if (found != begin)
				{
					spdlog::warn("{}: {} bytes skipped.", name(), found - begin);
				}
				// Adjust the new begin position for the search.
				begin = found + length;
				if (begin == end)
				{
					_bufferCount = 0;
				}
			}
			else
			{
				// If a start sequence is found, keep the rest in the buffer.
				if (start != nullptr)
				{
					if (start != begin)
					{
						spdlog::warn("{}: {} bytes skipped.", name(), start - begin);
					}
					std::memmove(_buffer, start, end - start);
					_bufferCount = end - start;
				}
				// No start sequence found.
				else
				{
					// Number of bytes left in the message.
					size_t left = end - begin;
					spdlog::warn("{}: {} bytes skipped.", name(), left);
					_bufferCount = 0;
				}
				break;
			}
		}
	}
}

// Find a frame in the provided buffer.
uint8_t* Nmea0183FramerFiber::findFrame(
	uint8_t*  begin,
	uint8_t*  end,
	size_t&   length,
	uint8_t*& start)
{
	static const uint8_t endSequence[] = { '\r', '\n' };
	uint8_t*             result{ nullptr };
	length = 0;
	start  = nullptr;

	// End shall be placed after begin.
	if (end > begin)
	{
		// Search the start sequence ($ or !).
		auto itStart = std::find_if(
			begin,
			end,
			[](uint8_t c) { return c == '$' || c == '!'; });

		// Search the end sequence when it is possible.
		if (itStart != end &&
			itStart + 1 < end)
		{
			auto itEnd = std::search(
				itStart + 1,
				end,
				endSequence,
				endSequence + 2);

			// Adjust the start position to remove start sequences without
			// end sequence ("$GPGGA,...$GPGSV,...*6c\r\n" -> "$GPGSV,...*6c\r\n).
			auto adjustStart = [](uint8_t*& start, uint8_t* end) {
				while (true)
				{
					auto nextStart = std::find_if(
						start + 1,
						end,
						[](uint8_t c) { return c == '$' || c == '!'; });

					if (nextStart == end)
					{
						break;
					}
					else
					{
						start = nextStart;
					}
				}
			};

			// End sequence found.
			if (itEnd != end)
			{
				adjustStart(itStart, itEnd);
				result = itStart;
				length = 2 + itEnd - itStart;
				start  = itStart;
			}
			// No end sequence found.
			else
			{
				adjustStart(itStart, end);
				start = itStart;
			}
		}
		// Start found but it is the only char of the message.
		else if (itStart != end)
		{
			start = itStart;
		}
	}

	return result;
}

} // namespace marine
} // namespace modules
} // namespace synapse
