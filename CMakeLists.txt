cmake_minimum_required(VERSION 3.7 FATAL_ERROR)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(NAME TrikytaEngine3D)
project(TrikytaEngine3D)

include_directories(${PROJECT_SOURCE_DIR})

# if (not defined ${CMAKE_BUILD_TYPE})
# 	set(CMAKE_BUILD_TYPE "Debug")
# endif()

# Set preprocessor defines
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNOMINMAX -D_USE_MATH_DEFINES")

# Compiler flags
if (MSVC)
	# add_compile_options(/W4 /WX)
else(MSVC)
	# add_compile_options(-Wpedantic -Wall -Wextra)
endif(MSVC)

# Clang specific stuff
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-switch-enum")
endif()

# Compiler specific stuff
IF(MSVC)
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ELSEIF(APPLE)
	if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -ObjC++")
	ELSE()
		SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fobjc-arc -xobjective-c++")
	ENDIF()
ENDIF(MSVC)

if (CMAKE_BUILD_TYPE MATCHES "Debug")
	# add_compile_definitions(DEBUG)
	add_definitions(-D_DEBUG=1)
	add_definitions(-DDEBUG=1)
	remove_definitions(-DNDEBUG)

	if (MSVC)
		# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi /DEBUG:FULL")
	else(MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
	endif()

	# message("Debug buildddd")
	#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
	#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

if (CMAKE_BUILD_TYPE MATCHES "Release")
	add_definitions(-DNDEBUG=1)
	remove_definitions(-D_DEBUG)
	remove_definitions(-DDEBUG)

	# set(CMAKE_CXX_FLAGS "-Ofast")
	# set(CMAKE_CXX_FLAGS "-g")
	if (MSVC)
		# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Oxyib")
	else(MSVC)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Ofast")
	endif()

	# message("Release buildddd a")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Libs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Libs)
set(MAIN_BINARY_DIR ${CMAKE_BINARY_DIR})
link_directories(${CMAKE_BINARY_DIR}/Libs)

add_subdirectory(Core)
add_subdirectory(Renderer)
add_subdirectory(Engine)

# set(BUILD_UNIT_TESTS true)

if (BUILD_UNIT_TESTS)
	if (MSVC)
		set(CMAKE_TOOLCHAIN_FILE "${CMAKE_BINARY_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")
	endif(MSVC)

	enable_testing()
	add_subdirectory(Tests/Tests)
endif(BUILD_UNIT_TESTS)

if (BUILD_BENCHMARKS)
	add_subdirectory(Tests/Benchmarks)
endif(BUILD_BENCHMARKS)
