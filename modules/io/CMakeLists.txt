cmake_minimum_required (VERSION 3.30.0)

project(synapse-modules-io
	VERSION 0.1.0
	DESCRIPTION "Synapse C++ IO module"
	LANGUAGES CXX)

# Package requirement.
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# List of source files of the library (excluding generated files).
set(SRC
	src/FileLoggerSink.cpp
	src/FramerFiber.cpp
	src/SerialSource.cpp
	src/TcpClientSource.cpp
	src/TcpServerSink.cpp
	src/main.cpp
	src/module.cpp)

# Definition of the library.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(synapse-modules-io SHARED ${SRC})

if(MSVC)
	target_compile_definitions(synapse-modules-io
		PRIVATE
		"_WIN32_WINNT=_WIN32_WINNT_WIN7")
endif()

target_link_libraries(synapse-modules-io
	PUBLIC
		synapse-framework
	PRIVATE
		Boost::boost
		fmt::fmt
		spdlog::spdlog)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS synapse-modules-io
	EXPORT  SynapseModulesIoConfig
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
