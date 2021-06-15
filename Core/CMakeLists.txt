project(Core)

SET(MODULE_FOLDER Core)
SET(MODULE_NAME Core)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MODULE_FOLDER})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")

# Sources
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS
    "*.h"
    "*.hpp"
    "*.cpp"
    "*.c"
    "*.inl"
)

if (MSVC)
    foreach(_source IN ITEMS ${SOURCE})
        get_filename_component(_source_path "${_source}" PATH)
        string(REPLACE "${MODULE_FOLDER}" "" _group_path "${_source_path}")
        source_group("${_group_path}" FILES "${_source}")
    endforeach()
endif(MSVC)

add_library(${MODULE_NAME} STATIC ${SOURCE})
set(CORE_LIB ${CMAKE_BINARY_DIR}/${MODULE_NAME})
# target_include_directories (Core PUBLIC include)
set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
