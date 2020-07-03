#Library to pack convex polygons into arbitrary shapes.
#Any copyright is dedicated to the public domain. See LICENSE.md for more details.

cmake_minimum_required(VERSION 3.16.3) #Oldest version it was tested with.
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake") #Finding dependencies.

#Release configuration.
set(CONVACK_VERSION_MAJOR 0 CACHE STRING "Major release version. This must be incremented if consumers of older versions may need to change their code.")
set(CONVACK_VERSION_MINOR 0 CACHE STRING "Minor release version. This must be incremented if consumers of older versions don't need to change their code, but consumers of newer versions may need to change their code to downgrade.")
set(CONVACK_VERSION_PATCH 0 CACHE STRING "Patch release version. This must be incremented for every release where there are no changes to the API.")

project(convack VERSION ${CONVACK_VERSION_MAJOR}.${CONVACK_VERSION_MINOR}.${CONVACK_VERSION_PATCH} DESCRIPTION "Library to pack convex polygons into arbitrary shapes.")

#Compiler requirements.
set(CMAKE_CXX_STANDARD 11)

#Where to find the source code.
set(convack_sources
	beam/beam_search.cpp
	beam/packing_candidate.cpp
	convex_polygon.cpp
	point2.cpp
	scene.cpp
	transformation.cpp
)
set(convack_source_paths "")
foreach(f IN LISTS convack_sources)
	list(APPEND convack_source_paths "${CMAKE_CURRENT_SOURCE_DIR}/src/${f}")
endforeach()

#The main target.
add_library(convack SHARED ${convack_source_paths})
target_include_directories(convack PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/convack")

#Automated tests.
option(BUILD_TESTS "Build tests to verify correctness of the library." OFF)
if(BUILD_TESTS)
	enable_testing()

	find_package(GoogleTest 1.10 REQUIRED)
	find_package(Threads REQUIRED) #Threading library is required for GoogleTest if it's compiled with default parameters.

	#The names of all tests. Each must have a file called "test/<name>.cpp" as the source file.
	#Instead of slashes for the directories, use periods.
	set(test_names
		convex_polygon
		transformation
	)

	#To make sure that the tests are built before running them, add the building of these tests as an additional test.
	add_custom_target(build_all_tests)
	add_test(BuildTests "${CMAKE_COMMAND}" --build ${CMAKE_CURRENT_BINARY_DIR} --target build_all_tests)

	include(GoogleTest) #Defines the gtest_discover_tests command.

	foreach(test_case ${test_names})
		#Expand periods to slashes for the path to the test case, since CMake can't handle slashes in target names.
		string(REPLACE . / test_path ${test_case})
		#Make sure that the directory exists.
		get_filename_component(test_dirname ${test_path} DIRECTORY)
		file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${test_dirname})

		add_executable(${test_case} "${CMAKE_CURRENT_SOURCE_DIR}/test/${test_path}.cpp")
		target_link_libraries(${test_case} PRIVATE convack ${GOOGLETEST_BOTH_LIBRARIES} "${CMAKE_THREAD_LIBS_INIT}")
		target_include_directories(${test_case} PRIVATE ${GOOGLETEST_INCLUDE_DIRS})
		add_dependencies(${test_case} GoogleTest)
		add_dependencies(build_all_tests ${test_case}) #Make sure that this gets built as part of the build_all_tests target.
		gtest_discover_tests(${test_case} PROPERTIES DEPENDS BuildTests)
	endforeach()
endif()