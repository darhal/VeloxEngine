SET(MODULE_FOLDER Tests)
SET(MODULE_NAME Tests)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MODULE_FOLDER})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")

find_package(Threads REQUIRED)
find_package(GTest REQUIRED)
IF (NOT GTEST_FOUND)
	message(FATAL_ERROR "Could not find Google Tests library!")
ENDIF()
include_directories(${GTEST_INCLUDE_DIR})


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

add_executable(${MODULE_NAME} ${SOURCE})
target_link_libraries(${MODULE_NAME} ${GTEST_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
# gtest_add_tests(TARGET ${MODULE_NAME} TEST_PREFIX)
gtest_discover_tests(${MODULE_NAME} TEST_PREFIX)
add_test(NAME ${MODULE_NAME} COMMAND ${MODULE_NAME})
set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
