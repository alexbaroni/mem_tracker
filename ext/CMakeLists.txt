cmake_minimum_required(VERSION 3.15)
project(mem_tracker_ext)
set(CMAKE_CXX_STANDARD 20)

set(EXT_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

#configure directories
set(TYPE_NAME_MODULE_PATH "${EXT_MODULE_PATH}/type_name")
set(TYPE_NAME_INCLUDE_PATH "${TYPE_NAME_MODULE_PATH}/include" PARENT_SCOPE)

add_subdirectory(type_name EXCLUDE_FROM_ALL)