cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT Renderer)

project(${PROJECT})

find_package(Vulkan REQUIRED)

add_subdirectory(libs/fmt ${CMAKE_BINARY_DIR}/libs/fmt)
add_subdirectory(libs/glm ${CMAKE_BINARY_DIR}/libs/glm)
add_subdirectory(libs/SDL ${CMAKE_BINARY_DIR}/libs/SDL)

file(GLOB_RECURSE SRC "src/*.cpp")

add_executable(${PROJECT} ${SRC})

target_compile_options(${PROJECT} PRIVATE --std=c++20 -g -O1 -Wall -Werror)
target_include_directories(${PROJECT} PRIVATE include libs/fmt/include libs/glm libs/SDL/include libs/vma/include) 
target_link_libraries(${PROJECT} PRIVATE Vulkan::Vulkan fmt::fmt glm::glm SDL3::SDL3)

file(GLOB_RECURSE DYNAMIC_LIBS ${CMAKE_BINARY_DIR}/libs/*.so)
file(COPY ${DYNAMIC_LIBS} DESTINATION ${CMAKE_BINARY_DIR})
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})
