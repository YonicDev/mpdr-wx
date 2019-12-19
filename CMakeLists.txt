cmake_minimum_required(VERSION 3.10)

# set the project name
project(MPDR VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# wxWidgets
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

# rust support
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeRust/cmake/")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MD")
enable_language(Rust)
include(CMakeCargo)

add_subdirectory(randomprime)

# add the executable
add_executable(MPDR ./DoorRandomizer/GUI/CApp.cpp)


# compiler definitions
add_definitions(-D_CRT_SECURE_NO_WARNINGS=1,_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING=1)


# library linkage
target_link_libraries(MPDR randomprime ws2_32 userenv ${wxWidgets_LIBRARIES})
target_include_directories(MPDR PUBLIC "${CMAKE_SOURCE_DIR}/include/rapidjson/include")
