cmake_minimum_required(VERSION 3.7)
project(vChip)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "-fpermissive -g -march=native -O0")
set(SOURCE_FILES
        main.cpp)

add_executable(vChip ${SOURCE_FILES})
target_link_libraries(vChip)