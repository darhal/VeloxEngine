SET(MODULE_FOLDER Engine)
SET(MODULE_NAME Engine)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")

include_directories(${MODULE_FOLDER})

# Sources
file(GLOB_RECURSE SOURCE CONFIGURE_DEPENDS
    "*.h"
    "*.hpp"
    "*.cpp"
    "*.c"
)

# wayland requires additional source files
IF(USE_WAYLAND_WSI)
	SET(SOURCE ${SOURCE} ${CMAKE_BINARY_DIR}/xdg-shell-client-protocol.h ${CMAKE_BINARY_DIR}/xdg-shell-protocol.c)
ENDIF()


if(WIN32)
    add_executable(${MODULE_NAME} ${SOURCE})
    target_link_libraries(${MODULE_NAME} ${Vulkan_LIBRARY} ${WINLIBS})
else(WIN32)
    # add_executable(${MODULE_NAME} ${SOURCE})
    # target_link_libraries(${MODULE_NAME})
endif(WIN32)

# set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
