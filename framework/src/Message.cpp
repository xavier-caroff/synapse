///
/// @file Message.cpp
///
/// Implementation of the Message class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <cstring>

#include "synapse/framework/Message.h"

namespace synapse {
namespace framework {

// Constructor.
Message::Message(
	size_t size)
	: _size(size)
{
	if (size > 0)
	{
		_payload = new uint8_t[size];
	}
}

// Constructor with initialization of the payload.
Message::Message(
	size_t         size,
	const uint8_t* payload)
	: _size(size)
{
	if (size > 0)
	{
		_payload = new uint8_t[size];
		std::memcpy(_payload, payload, size);
	}
}

// Move constructor.
Message::Message(
	Message&& other) noexcept
{
	_payload       = other._payload;
	_size          = other._size;
	other._payload = nullptr;
	other._size    = 0;
}

// Destructor.
Message::~Message()
{
	if (_payload != nullptr)
	{
		delete[] _payload;
		_payload = nullptr;
	}
}

// Move operator.
Message& Message::operator=(
	Message&& other) noexcept
{
	_payload       = other._payload;
	_size          = other._size;
	other._payload = nullptr;
	other._size    = 0;

	return *this;
}

} // namespace framework
} // namespace synapse