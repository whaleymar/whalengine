cmake_minimum_required(VERSION 3.13)
project(engine CXX C)
cmake_policy(SET CMP0079 NEW)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_DEBUG_POSTFIX d)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)

add_subdirectory(lib/glm) # needs to be before add_executable to avoid recursive variable-setting
# add_subdirectory(lib/LibNerve)
add_subdirectory(lib/glad)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


file(GLOB_RECURSE SOURCES_BASE ${PROJECT_SOURCE_DIR}/src/*.cpp ${PROJECT_SOURCE_DIR}/src/*.c ${PROJECT_SOURCE_DIR}/src/*.hpp ${PROJECT_SOURCE_DIR}/src/*.h)

add_executable(engine ${SOURCES_BASE})
set_target_properties(engine PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_include_directories(engine
    PUBLIC src
    PUBLIC "${PROJECT_SOURCE_DIR}/lib/glad/include"
    PUBLIC "${PROJECT_SOURCE_DIR}/lib/stb"
    PUBLIC "${PROJECT_SOURCE_DIR}/lib/RapidXML"
    PUBLIC "${PROJECT_SOURCE_DIR}/lib/SDL/include"
    PUBLIC "${SDL2_INCLUDE_DIRS}"
    # PUBLIC "${PROJECT_SOURCE_DIR}/lib/LibNerve/Include"
    )

target_compile_options(engine PRIVATE "$<$<CONFIG:DEBUG>:${-g}>")
target_compile_options(engine PRIVATE "$<$<CONFIG:RELEASE>:${-O2}>")
target_compile_options(engine PRIVATE -fno-rtti -fno-exceptions)
target_compile_options(engine PRIVATE -Wall -Wextra -Wdeprecated)
target_compile_options(engine PRIVATE -Wno-unused-parameter -Wno-unused-private-field)
target_compile_options(engine PRIVATE -fno-strict-aliasing)
target_compile_options(engine PRIVATE -Wno-invalid-offsetof)

include_directories(lib)

target_link_libraries(engine glad)
target_link_libraries(engine ${SDL2_LIBRARIES})
target_link_libraries(engine ${SDL2_MIXER_LIBRARIES})
target_link_libraries(engine glm)
# target_link_libraries(engine Nerve)

find_package(OpenGL REQUIRED)
target_link_libraries(engine OpenGL::GL)
