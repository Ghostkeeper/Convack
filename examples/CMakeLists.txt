#Library to pack convex polygons into arbitrary shapes.
#Any copyright is dedicated to the public domain. See LICENSE.md for more details.

cmake_minimum_required(VERSION 3.16.3) #Oldest version it was tested with.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake") #Finding dependencies.

#The examples are all single-file, for simplicity. These are the examples we have.
#Each of these files must have the extension .cpp and must contain a main() function.
set(examples
    regular_polygons
)

option(BUILD_EXAMPLES "Build sample programs that show how to use this library." OFF)
if(BUILD_EXAMPLES)
    foreach(example IN LISTS examples)
        set(example_path "${CMAKE_CURRENT_SOURCE_DIR}/${example}.cpp")
        add_executable(${example} ${example_path})
        target_link_libraries(${example} PRIVATE convack)
		target_include_directories(${example} PRIVATE "${CMAKE_SOURCE_DIR}/include")
    endforeach()
endif()