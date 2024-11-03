///
/// @file Nmea0183FramerFiberTest.cpp
///
/// Unit testing of the Nmea0183FramerFiber class.
///
/// @author Xavier Caroff <xavier.caroff@free.fr>
/// @copyright Copyright (c) 2024, Xavier Caroff
///

#include <gtest/gtest.h>

#include <synapse/framework/Message.h>

#include "Nmea0183FramerFiber.h"

namespace synapse {
namespace modules {
namespace marine {

TEST(Nmea0183FramerFiber, findFrame)
{
	// clang-format off
	static const char* DATA[] = {
		"$GPGGA,some data*6C\r\n",
		"gg*4C\r\n$GPGGA,some data*6C\r\n$GPGSV,4585",
		"gg*4C\r\n$GPGGA,some data$GPGGA,some data*6C\r\n$GPGSV,4585",
	};
	// clang-format on

	uint8_t* begin;
	uint8_t* end;
	size_t   length;
	uint8_t* start;
	uint8_t* result;

	// First case (21 chars)
	// $GPGGA,some data*6C\r\n

	begin  = (uint8_t*) (DATA[0]);
	end    = (uint8_t*) (DATA[0] + 21);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, begin);
	EXPECT_EQ(length, 21);
	EXPECT_EQ(start, result);

	begin  = (uint8_t*) (DATA[0]);
	end    = (uint8_t*) (DATA[0] + 15);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, begin);

	begin  = (uint8_t*) (DATA[0]);
	end    = (uint8_t*) (DATA[0] + 1);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, begin);

	begin  = (uint8_t*) (DATA[0] + 1);
	end    = (uint8_t*) (DATA[0] + 21);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, nullptr);

	begin  = (uint8_t*) (DATA[0] + 8);
	end    = (uint8_t*) (DATA[0] + 5);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, nullptr);

	begin  = (uint8_t*) (DATA[0] + 8);
	end    = (uint8_t*) (DATA[0] + 8);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, nullptr);

	// Second case (39 chars)
	// gg*4C\r\n$GPGGA,some data*6C\r\n$GPGSV,4585

	begin  = (uint8_t*) (DATA[1]);
	end    = (uint8_t*) (DATA[1] + 39);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, begin + 7);
	EXPECT_EQ(length, 21);
	EXPECT_EQ(start, result);

	begin  = (uint8_t*) (DATA[1]);
	end    = (uint8_t*) (DATA[1] + 39);
	result = Nmea0183FramerFiber::findFrame(
		begin, end - 11, length, start);
	EXPECT_EQ(result, begin + 7);
	EXPECT_EQ(length, 21);
	EXPECT_EQ(start, result);

	begin  = (uint8_t*) (DATA[1]);
	end    = (uint8_t*) (DATA[1] + 39);
	result = Nmea0183FramerFiber::findFrame(
		begin, end - 12, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, begin + 7);

	// Third case (55 chars)
	// gg*4C\r\n$GPGGA,some data$GPGGA,some data*6C\r\n$GPGSV,4585

	begin  = (uint8_t*) (DATA[2]);
	end    = (uint8_t*) (DATA[2] + 55);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, begin + 23);
	EXPECT_EQ(length, 21);
	EXPECT_EQ(start, begin + 23);

	begin  = (uint8_t*) (DATA[2]);
	end    = (uint8_t*) (DATA[2] + 38);
	result = Nmea0183FramerFiber::findFrame(
		begin, end, length, start);
	EXPECT_EQ(result, nullptr);
	EXPECT_EQ(length, 0);
	EXPECT_EQ(start, begin + 23);
}

TEST(Nmea0183FramerFiber, consume)
{
	class FakePort :
		public synapse::framework::IPort
	{
	public:

		void dispatch(
			const std::shared_ptr<synapse::framework::Message>& message) override final
		{
			messages.push_back(message);
		}

		std::list<std::shared_ptr<synapse::framework::Message>> messages;
	};

	class FakeManager :
		public synapse::framework::IManager
	{
	public:

		synapse::framework::IBlock* create(
			const std::string& name,
			const std::string& className) override final
		{
			return nullptr;
		}

		synapse::framework::IBlock* find(
			const std::string& name) const override final
		{
			return nullptr;
		}

		synapse::framework::IPort* find(
			synapse::framework::IBlock* block,
			const std::string&          name) const override final
		{
			return std::addressof(port);
		}

		mutable FakePort port;
	};

	static const size_t                   BLOCK_SIZE = 20;
	static const std::vector<std::string> DATA       = {
        "$SDDBT,38.0,f,11.6,M,06.3,F*3E\r\n",                                               // 1
        "$SDDPT,11.6,-1.0,99.0*7F\r\n",                                                     // 2
        "$HCHDG,331.3,00.0,E,00.0,E*40\r\n",                                                // 3
        "$WIMWV,025.0,R,016.3,N,A*20\r\n",                                                  // 4
        "$VWVHW,129.0,T,129.0,M,07.7,N,14.3,K*52\r\n",                                      // 5
        "$GPRMC,164517.59,A,4601.47709,N,00114.10553,W,0008.9,303.6,160316,0.0,W,A*05\r\n", // 6
        "$GPGGA,164517.59,4601.47709,N,00114.10553,W,1,05,0.0,5,M,50.0,M,,*61\r\n",         // 7
        "$GPGSA,A,3,02,03,14,23,31,,,,,,,,5.5,0.0,6.7*34\r\n",                              // 8
        "!AIVDM,1,1,,A,18vAfjo1hvwqN6PJHD8jHQrT050l,0*08\r\n",                              // 9
        "$SDDBT,37.2,f,11.3,M,06.2,F*37\r\n",                                               // 10
        "$SDDPT,11.3,-1.0,99.0*7A\r\n",                                                     // 11
        "$HCHDG,331.3,00.0,E,00.0,E*40\r\n",                                                // 12
        "$WIMWV,025.4,R,016.3,N,A*24\r\n",                                                  // 13
        "$VWVHW,129.0,T,129.0,M,07.7,N,14.3,K*52\r\n",                                      // 14
        "$GPRMC,164518.67,A,4601.47846,N,00114.10850,W,0008.9,303.6,160316,0.0,W,A*0D\r\n", // 15
        "$GPGGA,164518.67,4601.47846,N,00114.10850,W,1,05,0.0,5,M,50.0,M,,*69\r\n",         // 16
        "$GPGSA,A,3,02,03,14,23,31,,,,,,,,5.5,0.0,6.7*34\r\n",                              // 17
        "$SDDBT,39.3,f,12.0,M,06.6,F*3C\r\n",                                               // 18
        "$SDDPT,12.0,-1.0,99.0*7A\r\n",                                                     // 19
        "$HCHDG,331.3,00.0,E,00.0,E*40\r\n",                                                // 20
        "$WIMWV,025.0,R,016.5,N,A*26\r\n",                                                  // 21
        "$VWVHW,129.0,T,129.0,M,07.7,N,14.3,K*52\r\n",                                      // 22
        "$GPRMC,164519.78,A,4601.47983,N,00114.11147,W,0008.9,303.6,160316,0.0,W,A*04\r\n", // 23
        "$GPGGA,164519.78,4601.47983,N,00114.11147,W,1,05,0.0,5,M,50.0,M,,*60\r\n",         // 24
        "$GPGSA,A,3,02,03,14,23,31,,,,,,,,5.5,0.0,6.7*34\r\n",                              // 25
        "!AIVDM,1,1,,A,18vAfjo1hvwqN7pJHD<jHQr`050l,0*19\r\n",                              // 26
        "$SDDBT,38.7,f,11.8,M,06.5,F*31\r\n",                                               // 27
        "$SDDPT,11.8,-1.0,99.0*71\r\n",                                                     // 28
        "$HCHDG,331.3,00.0,E,00.0,E*40\r\n",                                                // 29
	};

	Nmea0183FramerFiber* object = dynamic_cast<Nmea0183FramerFiber*>(Nmea0183FramerFiber::create("object"));
	FakeManager          manager;
	nlohmann::json       config = {
        {"blockSize", 1024}
	};

	object->initialize(config, std::addressof(manager));

	auto all = std::accumulate(DATA.begin(), DATA.end(), std::string{});

	for (size_t idxBlock = 0; idxBlock < all.size(); idxBlock += BLOCK_SIZE)
	{
		auto length = std::min(BLOCK_SIZE, all.size() - idxBlock);
		auto msg    = std::make_shared<synapse::framework::Message>(length, (uint8_t*) all.data() + idxBlock);

		object->consume(msg);
	}

	object->destroy();
	object = nullptr;

	EXPECT_EQ(manager.port.messages.size(), DATA.size());
} // namespace marine

} // namespace marine
} // namespace modules
} // namespace synapse
