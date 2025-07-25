cmake_minimum_required(VERSION 4.0)
project(c.cpp-template-zed)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(c.cpp-template-zed ${SOURCE_FILES})

target_include_directories(c.cpp-template-zed PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
