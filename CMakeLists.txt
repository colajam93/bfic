cmake_minimum_required(VERSION 3.7)
project(bfic)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(DISABLE_CXX_FLAGS "-Wno-c++98-compat -Wno-padded -Wno-missing-prototypes")
set(CMAKE_CXX_FLAGS "-Werror -Weverything ${DISABLE_CXX_FLAGS}")

include_directories(include)
add_subdirectory(src)
