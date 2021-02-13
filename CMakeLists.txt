cmake_minimum_required(VERSION 3.10)

project(TrikytaEngine3D)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message("TrikytaEngine ${CMAKE_BUILD_TYPE} Build")

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    # add_compile_definitions(DEBUG)
    add_definitions(-D_DEBUG=1)
    remove_definitions(-DNDEBUG)

    set(CMAKE_CXX_FLAGS "-g")
    #set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
    #set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
endif()

if (CMAKE_BUILD_TYPE MATCHES "Release")
    add_definitions(-DNDEBUG=1)
    remove_definitions(-D_DEBUG)

    # set(CMAKE_CXX_FLAGS "-Ofast")
    set(CMAKE_CXX_FLAGS "-O3")
endif()

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
message("Build flags: ${CMAKE_CXX_FLAGS}")

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
