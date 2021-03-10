SET(MODULE_FOLDER Renderer)
SET(MODULE_NAME Renderer)

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

# TODO:
# set(SHADER_DIR_GLSL "../data/shaders/glsl/${EXAMPLE_NAME}")
# file(GLOB SHADERS_GLSL "${SHADER_DIR_GLSL}/*.vert" "${SHADER_DIR_GLSL}/*.frag" "${SHADER_DIR_GLSL}/*.comp" "${SHADER_DIR_GLSL}/*.geom" "${SHADER_DIR_GLSL}/*.tesc" "${SHADER_DIR_GLSL}/*.tese" "${SHADER_DIR_GLSL}/*.mesh" "${SHADER_DIR_GLSL}/*.task" "${SHADER_DIR_GLSL}/*.rgen" "${SHADER_DIR_GLSL}/*.rchit" "${SHADER_DIR_GLSL}/*.rmiss")
# source_group("Shaders\\GLSL" FILES ${SHADERS_GLSL})

set(SHADER_DIR_GLSL "")
set(README_FILES "")

if(WIN32)
    add_executable(${MODULE_NAME} ${SOURCE}) # ${SHADERS_GLSL} ${README_FILES}
    target_link_libraries(${MODULE_NAME} ${Vulkan_LIBRARY} ${WINLIBS})
else(WIN32)
    add_executable(${MODULE_NAME} ${SOURCE}) # ${SHADERS_GLSL} ${README_FILES}
    # target_link_libraries(${MODULE_NAME})
endif(WIN32)

set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})

message("debugger wd a: ${MODULE_FOLDER}")
