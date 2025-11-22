cmake_minimum_required(VERSION 3.25)
project(breakout VERSION 1.0.0)

option(FETCH_LIBS OFF)

if (FETCH_LIBS)
    include(FetchContent)

    FetchContent_Declare(
        box2d
        GIT_REPOSITORY https://github.com/erincatto/box2d.git
        GIT_TAG main
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(box2d)

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
    find_package(box2d REQUIRED)
endif(FETCH_LIBS)
unset(FETCH_LIBS CACHE)

add_subdirectory(external)
add_subdirectory(lib)
add_subdirectory(bin)

message(STATUS "Building ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
