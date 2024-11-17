///
/// @file Nmea0183RouterFiberTest.cpp
///
/// Unit testing of the Nmea0183RouterFiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <gtest/gtest.h>

#include <synapse/framework/Message.h>

#include "Nmea0183RouterFiber.h"

namespace synapse {
namespace modules {
namespace marine {

TEST(Nmea0183RouterFiber, node_initialize)
{
	{
		auto                       root    = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port    = reinterpret_cast<synapse::framework::IPort*>(12);
		Nmea0183RouterFiber::Node* current = root.get();

		current->initialize(reinterpret_cast<const uint8_t*>("$GPGGA"), 6, port);

		EXPECT_EQ(current->byte, '$');
		EXPECT_EQ(current->fallback.get(), nullptr);
		EXPECT_EQ(current->port, nullptr);
		EXPECT_NE(current->next.get(), nullptr);

		current = current->next.get();

		EXPECT_EQ(current->byte, 'G');
		EXPECT_EQ(current->fallback.get(), nullptr);
		EXPECT_EQ(current->port, nullptr);
		EXPECT_NE(current->next.get(), nullptr);

		current = current->next.get();

		EXPECT_EQ(current->byte, 'P');
		EXPECT_EQ(current->fallback.get(), nullptr);
		EXPECT_EQ(current->port, nullptr);
		EXPECT_NE(current->next.get(), nullptr);

		current = current->next.get();

		EXPECT_EQ(current->byte, 'G');
		EXPECT_EQ(current->fallback.get(), nullptr);
		EXPECT_EQ(current->port, nullptr);
		EXPECT_NE(current->next.get(), nullptr);

		current = current->next.get();

		EXPECT_EQ(current->byte, 'G');
		EXPECT_EQ(current->fallback.get(), nullptr);
		EXPECT_EQ(current->port, nullptr);
		EXPECT_NE(current->next.get(), nullptr);

		current = current->next.get();

		EXPECT_EQ(current->byte, 'A');
		EXPECT_EQ(current->fallback.get(), nullptr);
		EXPECT_EQ(current->port, port);
		EXPECT_EQ(current->next.get(), nullptr);
	}

	{
		auto                       root    = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port    = reinterpret_cast<synapse::framework::IPort*>(12);
		Nmea0183RouterFiber::Node* current = root.get();

		EXPECT_THROW(current->initialize(reinterpret_cast<const uint8_t*>(""), 0, port), std::logic_error);
	}

	{
		auto                       root    = std::make_unique<Nmea0183RouterFiber::Node>();
		Nmea0183RouterFiber::Node* current = root.get();

		EXPECT_THROW(current->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, nullptr), std::logic_error);
	}
}

TEST(Nmea0183RouterFiber, node_extend)
{
	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		synapse::framework::IPort* port2 = reinterpret_cast<synapse::framework::IPort*>(13);
		Nmea0183RouterFiber::Node* main  = root.get();

		main->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, port1);
		main->extend(reinterpret_cast<const uint8_t*>("!AI"), 3, port2);

		// Check the first chain $GP

		EXPECT_EQ(main->byte, '$');
		EXPECT_NE(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, nullptr);
		EXPECT_NE(main->next.get(), nullptr);

		main = main->next.get();

		EXPECT_EQ(main->byte, 'G');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, nullptr);
		EXPECT_NE(main->next.get(), nullptr);

		main = main->next.get();

		EXPECT_EQ(main->byte, 'P');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, port1);
		EXPECT_EQ(main->next.get(), nullptr);

		// Check the second chain !AI

		main = root->fallback.get();

		EXPECT_EQ(main->byte, '!');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, nullptr);
		EXPECT_NE(main->next.get(), nullptr);

		main = main->next.get();

		EXPECT_EQ(main->byte, 'A');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, nullptr);
		EXPECT_NE(main->next.get(), nullptr);

		main = main->next.get();

		EXPECT_EQ(main->byte, 'I');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, port2);
		EXPECT_EQ(main->next.get(), nullptr);
	}

	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		synapse::framework::IPort* port2 = reinterpret_cast<synapse::framework::IPort*>(13);
		Nmea0183RouterFiber::Node* main  = root.get();

		main->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, port1);
		main->extend(reinterpret_cast<const uint8_t*>("$GA"), 3, port2);

		// Check the first chain $GP

		EXPECT_EQ(main->byte, '$');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, nullptr);
		EXPECT_NE(main->next.get(), nullptr);

		main = main->next.get();

		EXPECT_EQ(main->byte, 'G');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, nullptr);
		EXPECT_NE(main->next.get(), nullptr);

		main = main->next.get();

		EXPECT_EQ(main->byte, 'P');
		EXPECT_NE(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, port1);
		EXPECT_EQ(main->next.get(), nullptr);

		// Check the second chain $GA

		main = main->fallback.get();

		EXPECT_EQ(main->byte, 'A');
		EXPECT_EQ(main->fallback.get(), nullptr);
		EXPECT_EQ(main->port, port2);
		EXPECT_EQ(main->next.get(), nullptr);
	}

	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		synapse::framework::IPort* port2 = reinterpret_cast<synapse::framework::IPort*>(13);
		Nmea0183RouterFiber::Node* main  = root.get();

		main->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, port1);

		EXPECT_THROW(main->extend(reinterpret_cast<const uint8_t*>("$"), 1, port2), std::runtime_error);
	}

	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		synapse::framework::IPort* port2 = reinterpret_cast<synapse::framework::IPort*>(13);
		Nmea0183RouterFiber::Node* main  = root.get();

		main->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, port1);

		EXPECT_THROW(main->extend(reinterpret_cast<const uint8_t*>("$GPG"), 1, port2), std::runtime_error);
	}

	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		synapse::framework::IPort* port2 = reinterpret_cast<synapse::framework::IPort*>(13);
		Nmea0183RouterFiber::Node* main  = root.get();

		main->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, port1);

		EXPECT_THROW(main->extend(reinterpret_cast<const uint8_t*>(""), 0, port2), std::runtime_error);
	}

	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		Nmea0183RouterFiber::Node* main  = root.get();

		main->initialize(reinterpret_cast<const uint8_t*>("$GP"), 3, port1);

		EXPECT_THROW(main->extend(reinterpret_cast<const uint8_t*>("!A"), 2, nullptr), std::logic_error);
	}
}

TEST(Nmea0183RouterFiber, node_match)
{
	{
		auto                       root  = std::make_unique<Nmea0183RouterFiber::Node>();
		synapse::framework::IPort* port1 = reinterpret_cast<synapse::framework::IPort*>(12);
		synapse::framework::IPort* port2 = reinterpret_cast<synapse::framework::IPort*>(13);
		synapse::framework::IPort* port3 = reinterpret_cast<synapse::framework::IPort*>(14);
		synapse::framework::IPort* port4 = reinterpret_cast<synapse::framework::IPort*>(15);

		root->initialize(reinterpret_cast<const uint8_t*>("$GPGGA"), 6, port1);
		root->extend(reinterpret_cast<const uint8_t*>("!AIVDM"), 6, port2);
		root->extend(reinterpret_cast<const uint8_t*>("$GPGST"), 6, port3);
		root->extend(reinterpret_cast<const uint8_t*>("$II"), 3, port4);

		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$GPG"), 4), nullptr);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("!"), 1), nullptr);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("!AIVDN"), 6), nullptr);

		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$GPGGA"), 6), port1);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$GPGGA,12,13"), 12), port1);

		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("!AIVDM"), 6), port2);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("!AIVDM,12,13"), 12), port2);

		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$GPGST"), 6), port3);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$GPGST,12,13"), 12), port3);

		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$II"), 3), port4);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$IIGST"), 6), port4);
		EXPECT_EQ(root->match(reinterpret_cast<const uint8_t*>("$IIGST,12,13"), 12), port4);
	}
}

} // namespace marine
} // namespace modules
} // namespace synapse
