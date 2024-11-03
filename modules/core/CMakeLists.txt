cmake_minimum_required (VERSION 3.30.0)

project(synapse-modules-core
	VERSION 0.1.0
	DESCRIPTION "Synapse C++ core module"
	LANGUAGES CXX)

# Package requirement.
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

# List of source files of the library (excluding generated files).
set(SRC
	src/module.cpp)

# Definition of the library.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(synapse-modules-core SHARED ${SRC})

target_link_libraries(synapse-modules-core
	PUBLIC
		synapse-framework
	PRIVATE
		Boost::boost
		fmt::fmt
		spdlog::spdlog)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS synapse-modules-core
	EXPORT  SynapseModulesIoConfig
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
