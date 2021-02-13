cmake_minimum_required(VERSION 3.10)

project(TrikytaEngine3D)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if (CMAKE_BUILD_TYPE EQUAL "DEBUG")
    # add_compile_definitions(DEBUG)
    add_definitions(-DDEBUG=1)
    message("TrikytaEngine DEBUG BUILD")
else()
    add_definitions(-DNDEBUG=1)
    message("TrikytaEngine RELEASE BUILD")
endif()

# set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")

set(CMAKE_CXX_FLAGS_RELEASE "-O3")

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
