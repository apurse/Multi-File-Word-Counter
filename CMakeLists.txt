cmake_minimum_required(VERSION 3.13)

# see https://vcpkg.io/en/docs/users/buildsystems/cmake-integration.html#visual-studio if using visual studio
# On Linux-based distrubtions, use your package manager for dependeices

# VCPKG integration for Windows. See my guide on getting that setup
if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
  set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "")
endif()

# Use C++ 17
set(CMAKE_CXX_STANDARD 17)

# Option support (faster build times by disabling features)
option(FAL_TESTS "Should we enable unit test support?" ON)
option(FAL_DOCS "Should we attempt to build documentation?" ON)

# Project details start here
project(MyProjectName
	VERSION 0.1
	DESCRIPTION "A brief description of your project here"
	LANGUAGES CXX
	)

# integration for command-line users
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

add_executable( project )
add_subdirectory( src )
target_include_directories(project PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
	$<INSTALL_INTERFACE:include/>
)

# Testing support
if (FAL_TESTS)
	include(CTest)
	add_subdirectory(tests)
endif()

# Documentation support
if (FAL_DOCS)
	find_package(Doxygen)
	if (Doxygen_FOUND)
		add_subdirectory( docs )
	else()
		message(STATUS "Doxygen not found, no docs will be built")
	endif()
endif()
