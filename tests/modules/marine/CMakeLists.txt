cmake_minimum_required (VERSION 3.30.0)

# Package requirement.
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(GTest REQUIRED)
find_package(spdlog REQUIRED)

# List of source files of the unit tests.
set(SRC
	../../../../modules/marine/src/Nmea0183FramerFiber.cpp
	src/Nmea0183FramerFiberTest.cpp)

# Definition of the unit test executable.
add_executable(synapse-test ${SRC})

target_link_libraries(synapse-test
	PRIVATE
		Boost::boost
		fmt::fmt
		GTest::GTest
		spdlog::spdlog
		synapse-framework)

target_include_directories(synapse-test
	PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/../../../modules/marine/src)