cmake_minimum_required(VERSION 3.10)

project(ParticlesDemo C)

set(CMAKE_C_STANDARD 11)

# Find SDL2
find_package(SDL2 REQUIRED)

add_executable(particles
    main.c
)

target_include_directories(particles PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(particles PRIVATE ${SDL2_LIBRARIES})