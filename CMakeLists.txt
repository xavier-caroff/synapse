cmake_minimum_required (VERSION 3.30.0)

# ------------------------------------------------------------------------------
# Option definitions
# ------------------------------------------------------------------------------

option(MsvcRuntimeDll	"Link dynamically to the MSVC runtime" ON)

# ------------------------------------------------------------------------------
# Project definitions
# ------------------------------------------------------------------------------

project(synapse
	VERSION 0.1.0
	DESCRIPTION "C++ framework for building configurable applications composed of blocks echanging messages along routes"
	LANGUAGES CXX)

# ------------------------------------------------------------------------------
# Adjust compiler settings for Conan
# ------------------------------------------------------------------------------

# Use libc++ instead of libstdc++ when using Clang
if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# Set compiler options to C++ 20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ------------------------------------------------------------------------------
# Conan stuff to get dependencies.
# ------------------------------------------------------------------------------

if(CONAN_EXPORTED) # in conan local cache
	# standard conan installation, deps will be defined in conanfile.py
	# and not necessary to call conan again, conan is already running
	include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
	conan_basic_setup()
else() # in user space

	# Select the MSVC runtime version to link with
	if (MSVC)
		if (MsvcRuntimeDll)
			set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
			set(CONAN_COMPILER_RUNTIME "compiler.runtime=MD")
		else (MsvcRuntimeDll)
			set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
			set(CONAN_COMPILER_RUNTIME "compiler.runtime=MT")
		endif (MsvcRuntimeDll)

		string(TOLOWER ${CMAKE_BUILD_TYPE} BuildType)
		if (BuildType STREQUAL "debug")
			set(CONAN_COMPILER_RUNTIME "${CONAN_COMPILER_RUNTIME}d")
		endif()
	endif(MSVC)

	# Get the conan.cmake file from the repository on Github
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
	list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})
	if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
		message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
		file(
			DOWNLOAD
				"https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
				"${CMAKE_BINARY_DIR}/conan.cmake"
			TLS_VERIFY
				ON)
	endif()

	# Adjust boost options when compiling on raspberry pi.
	if(LINUX)
		set(CONAN_OPTIONS
			boost:without_locale=True
			boost:without_stacktrace=True)
	endif()
	
	include(${CMAKE_BINARY_DIR}/conan.cmake)
	
	# Definition of conan dependencies
	conan_cmake_configure(
		SETTINGS
			${CONAN_COMPILER_RUNTIME}
		REQUIRES
			boost/1.86.0
			fmt/11.0.2
			gtest/1.15.0
			nlohmann_json/3.11.3
			spdlog/1.14.1
		OPTIONS
			${CONAN_OPTIONS}
		GENERATORS
			cmake_find_package)

	conan_cmake_autodetect(settings)

	conan_cmake_install(
		PATH_OR_REFERENCE
			.
		BUILD
			missing
		SETTINGS
			${settings})	
endif()

# Must use GNUInstallDirs to install libraries into correct
# locations on all platforms.
include(GNUInstallDirs)

if(WIN32 AND CMAKE_SIZEOF_VOID_P EQUAL 8)
	set(CMAKE_INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR}64)
	set(CMAKE_INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR}64)
endif()

# The file name of the debug version of the library must ends with 'd'.
set(CMAKE_DEBUG_POSTFIX d)

# Set compiler options to C++ 20 standard.
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Increase the warning level during build.
if(MSVC)
	add_definitions(
		-W4)
elseif(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(
		-Wall
		-Wextra)
endif()

# Source files are encoded in UTF-8
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# Output binaries and libs in standard locations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/bin") # .exe and .dll
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/lib") # .so and .dylib
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "$<0:>${CMAKE_BINARY_DIR}/lib") # .lib and .a

# ------------------------------------------------------------------------------
# Subdirectories
# ------------------------------------------------------------------------------

add_subdirectory(app)
add_subdirectory(framework)
add_subdirectory(modules)
add_subdirectory(tests/modules/marine)
