cmake_minimum_required(VERSION 3.10)

project(particle-simulation C)

set(CMAKE_C_STANDARD 11)

# Find SDL2
find_package(SDL2 REQUIRED)

add_executable(particle_simulation src/main.c)

target_include_directories(particle_simulation PRIVATE ${SDL2_INCLUDE_DIRS})
target_link_libraries(particle_simulation PRIVATE ${SDL2_LIBRARIES})