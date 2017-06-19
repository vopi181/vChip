cmake_minimum_required(VERSION 3.7)
project(vChip)

if (MSVC)
find_package(sfml:x64:windows)
link_directories(E:/vcpkg/installed/x64-windows/lib)
include_directories(E:/vcpkg/installed/x64-windows/include)
endif (MSVC)

if (UNIX)
message(STATUS ">>> LINUX DOESNT HAVE SFML BACKEND YET! Please Commit that asap")
endif (UNIX)

set(CMAKE_CXX_STANDARD 11)
if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)
set(CMAKE_CXX_FLAGS  "-fpermissive -g -march=native -O0")
endif (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUCC)



set(SOURCE_FILES
        main.cpp)



add_executable(vChip ${SOURCE_FILES})
target_link_libraries(vChip PUBLIC sfml-system)

