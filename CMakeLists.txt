cmake_minimum_required(VERSION 3.26.0)
project(breakout LANGUAGES CXX C)

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

add_library(flecs STATIC flecs.c)

# find_library(CIMGUI
#     NAMES cimgui.a cimgui
#     PATHS /usr/lib /usr/local/lib)

# if(NOT CIMGUI)
#     message(FATAL_ERROR "cimgui static library not found")
# endif()

# add_library(cimgui STATIC IMPORTED)
# set_target_properties(cimgui PROPERTIES IMPORTED_LOCATION ${CIMGUI})

find_package(logger REQUIRED)


# target_link_libraries(${PROJECT_NAME} PRIVATE raylib cimgui logger::logger)
add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib logger::logger flecs)

if (NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()

target_compile_options(${PROJECT_NAME} PRIVATE -std=c++0x)
