project(glad C)
cmake_minimum_required(VERSION 3.13)
set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)
include_directories(include ${OPENGL_INCLUDE_DIR})

# if (NOT WIN32)
#     find_package(LibDL REQUIRED)
#     include_directories(include ${LIBDL_INCLUDE_DIR})
# endif()

add_library(glad src/gl.c include/glad/gl.h)
# target_link_libraries(glad ${OPENGL_LIBRARIES} ${LIBDL_LIBRARIES})
target_link_libraries(glad ${OPENGL_LIBRARIES} )
