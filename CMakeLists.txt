cmake_minimum_required(VERSION 3.26.0)
project(breakout LANGUAGES CXX C)

add_library(flecs STATIC external/flecs.c)
target_include_directories(flecs PUBLIC external/)

option(FETCH_LIBS OFF)

if (FETCH_LIBS)
    include(FetchContent)

    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(raylib)
else()
    find_package(raylib REQUIRED)
endif(FETCH_LIBS)
unset(FETCH_LIBS CACHE)

find_package(logger REQUIRED)

file(GLOB_RECURSE COMPONENTS_SRC
    "src/components/*.h"
    "src/components/*.hpp"
    "src/components/*.cpp"
)

file(GLOB_RECURSE SYSTEMS_SRC
    "src/systems/*.h"
    "src/systems/*.hpp"
    "src/systems/*.cpp"
)

add_library(components STATIC ${COMPONENTS_SRC})
target_include_directories(components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/components)
target_link_libraries(components PUBLIC flecs PRIVATE raylib)

add_library(systems STATIC ${SYSTEMS_SRC})
target_include_directories(systems PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/systems)
target_link_libraries(systems PUBLIC components PRIVATE flecs raylib)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib logger::logger flecs components systems)

if (NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++0x)

# ---------------------------------
add_custom_target(copy_assets
    COMMAND ${CMAKE_COMMAND} -E echo "Force copying assets..."

    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/assets"
    "${CMAKE_CURRENT_BINARY_DIR}/"

    COMMENT "Force copying shaders and textures"
    VERBATIM
)

add_dependencies(${PROJECT_NAME} copy_assets)

set_target_properties(copy_assets PROPERTIES
    EXCLUDE_FROM_ALL FALSE
)
