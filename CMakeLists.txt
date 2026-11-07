cmake_minimum_required(VERSION 3.5)
project( main )

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(glad glad/src/glad.c)

target_include_directories(glad PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/glad/include"
)


add_executable( main main.cpp )

target_link_libraries( main PRIVATE glfw OpenGL::GL glad)
