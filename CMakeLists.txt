# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

set(JERRYSCRIPT_SOURCE "${CMAKE_SOURCE_DIR}/deps/jerryscript")

# Jerryscript setting here
set(JERRY_GLOBAL_HEAP_SIZE "(128)")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(nodemcujs)
