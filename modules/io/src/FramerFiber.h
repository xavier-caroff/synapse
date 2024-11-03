///
/// @file FramerFiber.h
///
/// Declaration of the FramerFiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

#include <cstdint>
#include <vector>

#include <synapse/framework/Fiber.h>
#include <synapse/framework/Port.h>

namespace synapse {
namespace modules {
namespace io {

///
/// Implement a block that extract from a raw stream messages based on a
/// start and end sequence of characters.
///
class FramerFiber :
	public synapse::framework::Fiber
{
	DECLARE_BLOCK(FramerFiber)

	// Définitions

public:

	/// Configuration of the block.
	struct Config
	{
		/// Start sequence of characters
		std::vector<uint8_t> start;
		/// End sequence of characters
		std::vector<uint8_t> end;
		/// Size of th buffer to store intermediate data.
		size_t               bufferSize;
	};

	/// Default size of the buffer to store intermediate data.
	static const size_t DEFAULT_BUFFER_SIZE{ 1024 };

	// Construction, destruction

private:

	/// Constructor.
	///
	/// @param name Name of the block.
	FramerFiber(
		const std::string& name);

	/// Destructor.
	virtual ~FramerFiber();

	// Implementation of IBlock

public:

	/// Initialize the block before the execution.
	///
	/// @param[in] configData The configuration data of the block.
	/// @param[in] manager The manager of the block.
	void initialize(
		const ConfigData&             configData,
		synapse::framework::IManager* manager) override;

	/// Ask the block to prepare to be deleted (terminate all pending operations).
	void shutdown() override final;

	// Implementation of IProducer

public:

	/// Get the list of output ports.
	std::list<std::string> ports() override final { return { OUTPUT_PORT_NAME }; }

	// Implementation of IConsumer

public:

	/// Consume a message.
	///
	/// @param message[in] Message to be consumed.
	void consume(
		const std::shared_ptr<synapse::framework::Message>& message) override final;

	// Operations

public:

	/// Replace escape sequences from a string.
	///
	/// @param[in] string The string to process.
	///
	/// @return The processed string.
	///
	/// @throw std::runtime_error When an invalid escape sequence is found.
	static std::vector<uint8_t> replaceEscapeSequences(
		const std::string& string);

	// Implementation

private:

	/// Find a frame in the provided buffer.
	///
	/// @param[in] begin The beginning of the buffer.
	/// @param[in] end The end of the buffer.
	/// @param[out] length The length of the frame.
	/// @param[out] start Set to the beginning of a frame if found.
	///
	/// @return The pointer to the beginning of the frame if found, nullptr otherwise.
	uint8_t* findFrame(
		uint8_t*  begin,
		uint8_t*  end,
		size_t&   length,
		uint8_t*& start) const;

	// Private definitions

private:

	/// Name of the output port.
	static const char* OUTPUT_PORT_NAME;

	// Private attributes

private:

	/// The configuration data.
	Config                     _config;

	/// The output port.
	synapse::framework::IPort* _outputPort{ nullptr };

	/// The buffer to store intermediate data.
	uint8_t*                   _buffer{ nullptr };

	/// The size of the buffer to store intermediate data.
	size_t                     _bufferSize{ 0 };

	/// The number of bytes in the buffer to store intermediate data.
	size_t                     _bufferCount{ 0 };
};

} // namespace io
} // namespace modules
} // namespace synapse
