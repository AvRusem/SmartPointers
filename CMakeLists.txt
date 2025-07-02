project(smart-pointers)

cmake_minimum_required(VERSION 3.16)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address,undefined -g -Wall -Wextra -pedantic -Werror -Wno-self-assign-overloaded -Wno-missing-braces")
set(CMAKE_EXPORT_COMPILE_COMMANDS  ON)

include_directories(.)
include_directories(control_block)
include_directories(unique_ptr)
include_directories(shared_ptr)
include_directories(weak_ptr)

add_subdirectory(unique_ptr)
add_subdirectory(weak_ptr)
add_subdirectory(shared_ptr)
