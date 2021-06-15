project(Renderer)

SET(MODULE_FOLDER Renderer)
SET(MODULE_NAME Renderer)

SET(MODULE_FOLDER ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${MODULE_FOLDER})
message(STATUS "Generating project file for example in ${MODULE_FOLDER}")

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

# TODO:
# set(SHADER_DIR_GLSL "../data/shaders/glsl/${EXAMPLE_NAME}")
# file(GLOB SHADERS_GLSL "${SHADER_DIR_GLSL}/*.vert" "${SHADER_DIR_GLSL}/*.frag" "${SHADER_DIR_GLSL}/*.comp" "${SHADER_DIR_GLSL}/*.geom" "${SHADER_DIR_GLSL}/*.tesc" "${SHADER_DIR_GLSL}/*.tese" "${SHADER_DIR_GLSL}/*.mesh" "${SHADER_DIR_GLSL}/*.task" "${SHADER_DIR_GLSL}/*.rgen" "${SHADER_DIR_GLSL}/*.rchit" "${SHADER_DIR_GLSL}/*.rmiss")
# source_group("Shaders\\GLSL" FILES ${SHADERS_GLSL})

set(SHADER_DIR_GLSL "")
set(README_FILES "")

add_executable(${MODULE_NAME} ${SOURCE}) # ${SHADERS_GLSL} ${README_FILES}
set_target_properties(${MODULE_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${MODULE_FOLDER})
add_dependencies(${MODULE_NAME} Core RendererBackend)
target_link_libraries(${MODULE_NAME} Core RendererBackend)
set(RENDERER_LIB ${CMAKE_BINARY_DIR}/${MODULE_NAME})
