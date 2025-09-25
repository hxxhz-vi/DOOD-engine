cmake_minimum_required(VERSION 4.0)
project(dood-engine)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 23)

file(GLOB SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_EXPERIMENTAL_CXX_MODULE_CMAKE_API "OFF")

add_compile_options(-Wall -Wextra -Wpedantic -g)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_executable(dood-engine ${SOURCE_FILES})

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

target_link_libraries(dood-engine PRIVATE ${SDL2_LIBRARIES})

target_include_directories(dood-engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
