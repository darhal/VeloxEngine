SET(MODULE_FOLDER Engine)
SET(MODULE_NAME Engine)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MODULE_FOLDER})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")

# Sources
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS
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

# wayland requires additional source files
IF(USE_WAYLAND_WSI)
	SET(SOURCE ${SOURCE} ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c)
ENDIF()

if(WIN32)
    # add_executable(${MODULE_NAME} ${SOURCE})
    # target_link_libraries(${MODULE_NAME} ${Vulkan_LIBRARY} ${WINLIBS})
else(WIN32)
    # add_executable(${MODULE_NAME} ${SOURCE})
    # target_link_libraries(${MODULE_NAME})
endif(WIN32)

# set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
# add_dependencies(${MODULE_NAME} Core RenderBackend Renderer)