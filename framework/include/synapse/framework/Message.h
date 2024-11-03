///
/// @file Message.h
///
/// Declaration of the Message class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///
#pragma once

namespace synapse {
namespace framework {

///
/// A message transmitted between blocks along a route.
///
class Message
{
	// Construction, destruction

public:

	/// Constructor without initialization of the payload.
	///
	/// @param size The size of the payload in bytes.
	Message(
		size_t size);

	/// Constructor with initialization of the payload.
	///
	/// @param size The size of the payload in bytes.
	/// @param payload The payload to be copied.
	Message(
		size_t         size,
		const uint8_t* payload);

	/// Move constructor.
	///
	/// @param other Object to be acquired.
	Message(
		Message&& other) noexcept;

	/// Destructor.
	virtual ~Message();

	// Accessors

public:

	/// Access to the payload of the message.
	///
	/// @return The payload of the message.
	const uint8_t* payload() const { return _payload; }

	/// Access to the payload of the message.
	///
	/// @return The payload of the message.
	uint8_t*       payload() { return _payload; }

	/// Size of the payload.
	///
	/// @return The size of the payload in bytes.
	size_t         size() const { return _size; }

	// Operators

public:

	/// Move operator.
	///
	/// @param other Object to be moved.
	///
	/// @return This object.
	Message& operator=(
		Message&& other) noexcept;

	// Private attributes

private:

	/// The payload of the message.
	uint8_t* _payload{ nullptr };

	/// The size of the payload of the message.
	size_t   _size{ 0 };
};

} // namespace framework
} // namespace synapse