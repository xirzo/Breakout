add_executable(${PROJECT_NAME} main.c)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib box2d tomlc17)

# NOTE: this allows to turn of debugging in debug build
target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:DEBUGGING>)

if (NOT MSVC)
    target_link_libraries(${PROJECT_NAME} PRIVATE m)
endif()
