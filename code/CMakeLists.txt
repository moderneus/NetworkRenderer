cmake_minimum_required(VERSION 4.2)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_C_COMPILER clang)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT NetworkRenderer)

project(${PROJECT})

add_subdirectory(libs/fmt ${CMAKE_BINARY_DIR}/libs/fmt)
add_subdirectory(libs/glm ${CMAKE_BINARY_DIR}/libs/glm)
add_subdirectory(libs/SDL ${CMAKE_BINARY_DIR}/libs/SDL)
add_subdirectory(libs/volk ${CMAKE_BINARY_DIR}/libs/volk)

add_subdirectory(server)
add_subdirectory(client)

if(WIN32)
	file(GLOB_RECURSE DYNAMIC_LIBS ${CMAKE_BINARY_DIR}/libs/*.dll)
elseif(LINUX)
	file(GLOB_RECURSE DYNAMIC_LIBS ${CMAKE_BINARY_DIR}/libs/*.so)
endif()

file(COPY ${DYNAMIC_LIBS} DESTINATION ${CMAKE_BINARY_DIR})
