cmake_minimum_required(VERSION 3.15)
project(mem_tracker)

set(CMAKE_CXX_STANDARD 20)
add_compile_options("$<$<CONFIG:DEBUG>:-O0;-fno-inline;-g3;-fno-omit-frame-pointer;-fno-stack-protector;-mieee-fp>")
add_compile_options("$<$<CONFIG:RELEASE>:-O3;-g0;-DNDEBUG>")
add_compile_options("$<$<CONFIG:RELWITHDEBINFO>:-O2;-g;-DNDEBUG>")

set(MEM_TRACKER_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(ext)

add_library(mem_tracker INTERFACE)
target_sources(mem_tracker INTERFACE
        ${MEM_TRACKER_INCLUDE_PATH}/mem_tracker/tracker.hpp)
target_link_libraries(mem_tracker INTERFACE type_name)
target_include_directories(mem_tracker INTERFACE ${MEM_TRACKER_INCLUDE_PATH} INTERFACE ${TYPE_NAME_INCLUDE_PATH})
target_include_directories(mem_tracker INTERFACE ${MEM_TRACKER_INCLUDE_PATH})

enable_testing()
include(CTest)
add_subdirectory(test)