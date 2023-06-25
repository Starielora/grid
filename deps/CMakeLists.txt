cmake_minimum_required(VERSION 3.22)

# glad
add_library(glad STATIC
    glad/include/glad/glad.h
    glad/include/KHR/khrplatform.h
    glad/src/glad.c
    )

target_include_directories(glad PUBLIC glad/include)

add_library(PerlinNoise INTERFACE)
target_include_directories(PerlinNoise INTERFACE PerlinNoise)

add_subdirectory(glm)
add_subdirectory(glfw)
