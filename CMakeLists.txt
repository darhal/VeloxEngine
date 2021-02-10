cmake_minimum_required(VERSION 3.10)

project(TrikytaEngine3D)

set(CMAKE_CXX_STANDARD 17)

#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB_RECURSE RENDERER_SRC CONFIGURE_DEPENDS
    "Renderer/*.h"
    "Renderer/*.hpp"
    "Renderer/*.cpp"
    "Renderer/*.c"
)

file(GLOB_RECURSE ENGINE_SRC CONFIGURE_DEPENDS
    "Engine/*.h"
    "Engine/*.hpp"
    "Engine/*.cpp"
    "Engine/*.c"
)

add_executable(Renderer ${RENDERER_SRC})
target_link_libraries(Renderer -lpthread -lm -lXrandr -lX11 -lvulkan)

# add_executable(Engine ${ENGINE_SRC})
