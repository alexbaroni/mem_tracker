cmake_minimum_required(VERSION 3.12)
project(mem_tracker_test)
set(CMAKE_CXX_STANDARD 20)

#configure directories
set(CATCH_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/catch2")

#include custom cmake function
include("${CATCH_INCLUDE_PATH}/ParseAndAddCatchTests.cmake")

add_library(catch2 INTERFACE)
target_sources(catch2 INTERFACE ${CATCH_INCLUDE_PATH}/catch.hpp)
target_include_directories(catch2 INTERFACE ${CATCH_INCLUDE_PATH})

enable_testing()

set(SOURCES_MEM_TRACKER_MAIN tracker_main.cpp)
set(SOURCES_TRACKER_TEST ${SOURCES_MEM_TRACKER_MAIN} tracker_test.cpp)
set(SOURCES_TRACKER_ALL_TEST ${SOURCES_MEM_TRACKER_MAIN} tracker_test.cpp)


add_executable(test.tracker.tracker_test ${SOURCES_TRACKER_TEST})
target_link_libraries(test.tracker.tracker_test INTERFACE mem_tracker INTERFACE catch2)
target_include_directories(test.tracker.tracker_test PRIVATE ${MEM_TRACKER_INCLUDE_PATH} ${CATCH_INCLUDE_PATH})

add_executable(test.mem_tracker.all_test ${SOURCES_TRACKER_TEST})
target_link_libraries(test.mem_tracker.all_test INTERFACE mem_tracker INTERFACE catch2)
target_include_directories(test.mem_tracker.all_test PRIVATE ${MEM_TRACKER_INCLUDE_PATH} ${CATCH_INCLUDE_PATH})

add_test(test.tracker.tracker_test test.tracker.tracker_test)
add_test(test.mem_tracker.all_test test.mem_tracker.all_test)
