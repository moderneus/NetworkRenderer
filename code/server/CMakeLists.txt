cmake_minimum_required(VERSION 4.2)

set(PROJECT Server)

project(${PROJECT})

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT} ${SRC})

target_compile_options(${PROJECT} PRIVATE --std=c++20 -g -O1 -Wall -Werror)
target_include_directories(${PROJECT} PRIVATE include ../libs/fmt/include ../libs/glm ../libs/SDL/include ../libs/vma/include ../libs/volk)
target_link_libraries(${PROJECT} PRIVATE fmt::fmt glm::glm SDL3::SDL3 volk::volk)
