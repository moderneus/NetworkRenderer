cmake_minimum_required(VERSION 4.2)

set(PROJECT Client)

project(${PROJECT})

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT} ${SRC})

target_compile_options(${PROJECT} PRIVATE --std=c++20 -g -O1 -Wall -Werror)
target_include_directories(${PROJECT} PRIVATE include ../libs/fmt/include ../libs/SDL/include) 
target_link_libraries(${PROJECT} PRIVATE fmt::fmt SDL3::SDL3)
