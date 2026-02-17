cmake_minimum_required(VERSION 4.0)
project(test_proj LANGUAGES C)

set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(
  -Wall -Wextra -Wpedantic -Wconversion -Wshadow -Werror
)
add_subdirectory(p1_calls)

