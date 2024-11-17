cmake_minimum_required (VERSION 3.30.0)

project(synapse-app-msg-server
	VERSION 0.1.0
	DESCRIPTION "Synapse C++ Message Server application"
	LANGUAGES CXX)

# Package requirement.
find_package(Boost REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)

# List of source files of the library (excluding generated files).
set(SRC
	src/Application.cpp
	src/HumanUI.cpp
	src/JsonUI.cpp
	src/main.cpp
	src/Server.cpp)

# Definition of the executable.

add_executable(synapse-app-msg-server ${SRC})

if(MSVC)
	target_compile_definitions(synapse-app-msg-server
		PRIVATE
		"_WIN32_WINNT=_WIN32_WINNT_WIN7")
endif()

target_link_libraries(synapse-app-msg-server
	PRIVATE
		Boost::program_options
		fmt::fmt
		nlohmann_json::nlohmann_json
		spdlog::spdlog
		synapse-framework)
	
# Simplify the name of the executable.
set_target_properties(synapse-app-msg-server PROPERTIES OUTPUT_NAME "msg-server")

# Change location and name of the pdb file (msvc).
set_target_properties(synapse-app-msg-server PROPERTIES
	COMPILE_PDB_NAME "msg-server"
	COMPILE_PDB_OUTPUT_DIR ${CMAKE_BINARY_DIR})

# 'make install' to the correct locations (provided by GNUInstallDirs).
install(TARGETS synapse-app-msg-server
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})  # This is for Windows
