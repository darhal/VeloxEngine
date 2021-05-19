SET(MODULE_FOLDER Benchmarks)
SET(MODULE_NAME Benchmarks)

include_directories(${MODULE_FOLDER})

if (MSVC_IDE)
    # find_package(GTest REQUIRED)
    # include_directories(${GTEST_INCLUDE_DIR})
else(MSVC_IDE)
    find_package(Threads REQUIRED)
    find_package(benchmark REQUIRED)
    include_directories(${BENCHMARK_INCLUDE_DIR})
endif(MSVC_IDE)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")
include(GoogleTest)

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

if(WIN32)
    add_executable(${MODULE_NAME} ${SOURCE})
    if (MSVC_IDE)
        target_link_libraries(${MODULE_NAME} ${CMAKE_THREAD_LIBS_INIT} ${WINLIBS})
    else(MSVC_IDE)
        target_link_libraries(${MODULE_NAME} benchmark::benchmark ${CMAKE_THREAD_LIBS_INIT} ${WINLIBS}) #
    endif(MSVC_IDE)
else(WIN32)
    add_executable(${MODULE_NAME} ${SOURCE})
    target_link_libraries(${MODULE_NAME} benchmark::benchmark ${CMAKE_THREAD_LIBS_INIT})
endif(WIN32)

set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
