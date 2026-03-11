cmake_minimum_required(VERSION 3.16)

project(olymp_algos CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/cpp)

add_executable(trie_example
    examples/data_structures/trie_example.cpp
)
