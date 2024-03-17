cmake_minimum_required(VERSION 3.13)
project(engine CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "-Wall -g")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES_BASE ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.hpp ${PROJECT_SOURCE_DIR}/src/*.h)
file(GLOB_RECURSE SOURCES_TEST ${PROJECT_SOURCE_DIR}/test/*.cpp ${PROJECT_SOURCE_DIR}/test/*.c ${PROJECT_SOURCE_DIR}/test/*.h ${PROJECT_SOURCE_DIR}/test/*.hpp)

add_executable(engine ${SOURCES_BASE} ${SOURCES_TEST})

target_include_directories(engine PRIVATE src)

target_compile_options(engine PRIVATE -fno-rtti -fno-exceptions)
target_compile_options(engine PRIVATE -Wall -Wextra -Wdeprecated)
target_compile_options(engine PRIVATE -Wno-unused-parameter -Wno-unused-private-field)
target_compile_options(engine PRIVATE -fno-strict-aliasing)
target_compile_options(engine PRIVATE -Wno-invalid-offsetof)

include_directories(${PROJECT_SOURCE_DIR}/src)

