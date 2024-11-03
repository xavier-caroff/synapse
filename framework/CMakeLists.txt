cmake_minimum_required (VERSION 3.30.0)

project(synapse-framework
	VERSION 0.1.0
	DESCRIPTION "Synapse C++ Framework"
	LANGUAGES CXX)

# Package requirement.
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)

# Generate version information in sources from this project information.
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/src/VersionInfo.cpp.in
	${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.cpp)

# List of source files of the library (excluding generated files).
set(SRC
	src/BaseBlock.cpp
	src/Dispatcher.cpp
	src/Fiber.cpp
	src/Manager.cpp
	src/Message.cpp
	src/Port.cpp
	src/Registry.cpp
	src/Route.cpp
	src/Sink.cpp
	src/Source.cpp
	${CMAKE_CURRENT_BINARY_DIR}/VersionInfo.cpp)

# Definition of the library.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(synapse-framework ${SRC})

# Precompilated header feature to speed up the build process.
target_precompile_headers(synapse-framework
	PRIVATE
		# Standard Template Library
		<list>
		<map>
		<memory>
		<stdexcept>
		<string>
		<type_traits>
		# {fmt}
		<fmt/format.h>
		# Nlohmann JSON
		<nlohmann/json.hpp>)

target_link_libraries(synapse-framework
	PUBLIC
		nlohmann_json::nlohmann_json
	PRIVATE
		Boost::boost
		Boost::filesystem
		fmt::fmt
		spdlog::spdlog)
	
# Include the version number in the file name of the library.
set_target_properties(synapse-framework PROPERTIES OUTPUT_NAME "${PROJECT_NAME}-${PROJECT_VERSION}")

# Change location and name of the pdb file (msvc).
set_target_properties(synapse-framework PROPERTIES
	COMPILE_PDB_NAME "${PROJECT_NAME}-${PROJECT_VERSION}${CMAKE_DEBUG_POSTFIX}"
	COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR})

# Define headers for this library. PUBLIC headers are used for
# compiling the library, and will be added to consumers' build
# paths.
target_include_directories(synapse-framework
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
		$<INSTALL_INTERFACE:include>
	PRIVATE
		src)

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS synapse-framework
	EXPORT  SynapseFrameworkConfig
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Deploy pdb file to avoid warning when linking with the library (msvc)
install(FILES "$<TARGET_FILE_DIR:${PROJECT_NAME}>/${PROJECT_NAME}-${PROJECT_VERSION}${CMAKE_DEBUG_POSTFIX}.pdb" DESTINATION ${CMAKE_INSTALL_LIBDIR} OPTIONAL)

# This makes the project importable from the install directory
# Put config file in per-project dir (name MUST match), can also
# just go into 'cmake'.
install(EXPORT SynapseFrameworkConfig DESTINATION share/SynapseFrameworkConfig/cmake)

# This makes the project importable from the build directory
export(TARGETS synapse-framework FILE SynapseFrameworkConfig.cmake)

# This generates the config version file.
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${CMAKE_BINARY_DIR}/SynapseFrameworkConfigVersion.cmake
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY ExactVersion)
install(FILES ${CMAKE_BINARY_DIR}/SynapseFrameworkConfigVersion.cmake DESTINATION share/SynapseFrameworkConfig/cmake)
