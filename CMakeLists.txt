cmake_minimum_required(VERSION 3.10)

# set the project name
project(MPDR VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# rust support
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeRust/cmake/")
enable_language(Rust)
include(CMakeCargo)

# add the executable
add_executable(MPDR App.cpp)

# library linkage
target_link_libraries(MPDR randomprime ws2_32 userenv)
