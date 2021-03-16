SET(MODULE_FOLDER Tests)
SET(MODULE_NAME Tests)

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)

enable_testing()

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")

include_directories(${MODULE_FOLDER})
include_directories(${GTEST_INCLUDE_DIR})

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
    add_executable(${MODULE_NAME} ${SOURCE} ${GTEST_LIBRARY} Threads::Threads)
	add_test(NAME ${MODULE_NAME} COMMAND ${MODULE_NAME})
else(WIN32)
    add_executable(${MODULE_NAME} ${SOURCE} ${GTEST_LIBRARY} Threads::Threads)
	add_test(NAME ${MODULE_NAME} COMMAND ${MODULE_NAME})
endif(WIN32)

set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
