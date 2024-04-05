cmake_minimum_required(VERSION 3.13)
project(engine CXX C)
cmake_policy(SET CMP0079 NEW)

add_subdirectory(lib/glm) # needs to be here to avoid recursive variable-setting
# add_subdirectory(lib/LibNerve)
add_subdirectory(lib/glad)
add_subdirectory(lib/glfw)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-Wall -g")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES_BASE ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.hpp ${PROJECT_SOURCE_DIR}/src/*.h)

add_executable(engine ${SOURCES_BASE})

target_include_directories(engine
    PUBLIC src
    PUBLIC "${PROJECT_SOURCE_DIR}/lib/glad/include"
    # PUBLIC "${PROJECT_SOURCE_DIR}/lib/LibNerve/Include"
    )

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)


target_compile_options(engine PRIVATE -fno-rtti -fno-exceptions)
target_compile_options(engine PRIVATE -Wall -Wextra -Wdeprecated)
target_compile_options(engine PRIVATE -Wno-unused-parameter -Wno-unused-private-field)
target_compile_options(engine PRIVATE -fno-strict-aliasing)
target_compile_options(engine PRIVATE -Wno-invalid-offsetof)

include_directories(lib)

target_link_libraries(engine glad)
target_link_libraries(engine glfw)
target_link_libraries(engine glm)
# target_link_libraries(engine Nerve)

find_package(OpenGL REQUIRED)
target_link_libraries(engine OpenGL::GL)
