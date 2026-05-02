cmake_minimum_required(VERSION 3.21)
project(OpenEngine VERSION 0.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# GLFW (windows and input) and glm (maths) are fetched and built with the
# project. Only X11 is enabled on Linux, to keep the build dependencies small.
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
    GIT_SHALLOW TRUE)

set(GLM_BUILD_LIBRARY OFF CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
    GIT_SHALLOW TRUE)

FetchContent_MakeAvailable(glfw glm)

# OpenGL 3.3 core loader, generated with glad (https://glad.dav1d.de).
add_library(glad STATIC third_party/glad/src/glad.c)
target_include_directories(glad PUBLIC third_party/glad/include)
target_link_libraries(glad PUBLIC ${CMAKE_DL_LIBS})

add_library(openengine STATIC
    engine/Camera.cpp
    engine/Mesh.cpp
    engine/Screenshot.cpp
    engine/Shader.cpp
    engine/Texture.cpp
    engine/Window.cpp)
target_include_directories(openengine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(openengine PUBLIC glad glfw glm::glm)
if(MSVC)
    target_compile_options(openengine PRIVATE /W4 /permissive-)
else()
    target_compile_options(openengine PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(demo demo/main.cpp)
target_link_libraries(demo PRIVATE openengine)

# The demo loads its shaders from a folder next to the executable.
add_custom_command(TARGET demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/shaders $<TARGET_FILE_DIR:demo>/shaders)
set_property(TARGET demo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY $<TARGET_FILE_DIR:demo>)
