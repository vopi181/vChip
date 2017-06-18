cmake_minimum_required(VERSION 3.7)
project(vChip)

include(ExternalProject)
set(SFML "${CMAKE_CURRENT_BINARY_DIR}/sfml")

#[[externalproject_add(sfml_project
		GIT_REPOSITORY "https://github.com/SFML/SFML.git"
		PREFIX "${CMAKE_CURRENT_BINARY_DIR/sfml_project}"
		CMAKE_ARGS
		"-G${CMAKE_GENERATOR}"
		"-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
		"-DCMAKE_INSTALL_PREFIX=${SFML}"
		)
		target_link_libraries(vChip sfml-system)
		include_directories(${SFML}/include)
		set(SFML_INCLUDE_PATH 'E:/git/SFML/include/')
		add_library(sfml-system STATIC IMPORTED)
set_target_properties(sfml-system PROPERTIES IMPORTED_LOCATION ${SFML}/lib/libsfml-system.a)
add_dependencies(sfml-system sfml_project)

]]
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS  "-fpermissive -g -march=native -O0")


set(SOURCE_FILES
        main.cpp)



add_executable(vChip ${SOURCE_FILES})

