cmake_minimum_required(VERSION 3.1)

find_package(mgstringref 0.0.1 REQUIRED HINTS ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(tests)
