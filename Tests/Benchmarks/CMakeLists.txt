SET(MODULE_FOLDER Benchmarks)
SET(MODULE_NAME Benchmarks)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MODULE_FOLDER})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")
include(GoogleTest)


find_package(benchmark REQUIRED)
IF (NOT benchmark_FOUND)
	message(FATAL_ERROR "Could not find Google Benchmark library!")
ENDIF()
include_directories(${BENCHMARK_INCLUDE_DIR})


# Sources
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS  LIST_DIRECTORIES false
    "*.h"
    "*.hpp"
    "*.cpp"
    "*.c"
)

if (MSVC)
    foreach(_source IN ITEMS ${SOURCE})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "${MODULE_FOLDER}" "" _group_path "${_source_path}")
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endif(MSVC)

add_executable(${MODULE_NAME} ${SOURCE})
target_link_libraries(${MODULE_NAME} benchmark::benchmark ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
