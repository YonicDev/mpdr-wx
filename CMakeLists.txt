cmake_minimum_required(VERSION 3.5)

# set the project name
project(MPDR VERSION 0.1)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

### add project files
# find headers and source files
file(GLOB_RECURSE MPDR_SOURCES "DoorRandomizer/*.cpp")
file(GLOB_RECURSE MPDR_HEADERS "DoorRandomizer/*.h")

set(MPDR_DIRS "")
foreach (_headerFile ${MPDR_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list(APPEND MPDR_DIRS ${_dir})
endforeach()

# add the executable
add_executable(MPDR ${MPDR_SOURCES})
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT MPDR)

### wxWidgets
set(wxWidgets_CONFIGURATION mswu)
set(wxWidgets_USE_LIBS)
find_package(wxWidgets COMPONENTS core base REQUIRED)
include(${wxWidgets_USE_FILE})

### randomprime

include(ExternalProject)

# Set default ExternalProject root directory
set_directory_properties(PROPERTIES EP_PREFIX ${CMAKE_BINARY_DIR}/Rust)

# add randomprime as a CMake target
ExternalProject_Add(
    randomprime
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND cargo build COMMAND cargo build --release
    BINARY_DIR "${CMAKE_SOURCE_DIR}/randomprime"
    INSTALL_COMMAND ""
    USES_TERMINAL_BUILD ON
    LOG_BUILD ON)

add_dependencies(MPDR randomprime)

# compiler definitions
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=1,_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING=1)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /MD")
endif()

# library linkage
set(RANDOMPRIME_LIB $<IF:$<CONFIG:Debug>,debug,release>)
if(MSVC)
    target_link_libraries(MPDR
        "${CMAKE_SOURCE_DIR}/randomprime/target/${RANDOMPRIME_LIB}/randomprime.lib"
        ws2_32 userenv advapi32 ${wxWidgets_LIBRARIES})
elseif(UNIX)
    target_link_libraries(MPDR
        "${CMAKE_SOURCE_DIR}/randomprime/target/${RANDOMPRIME_LIB}/librandomprime.a"
        libdl.so X11 ${wxWidgets_LIBRARIES})
endif()
#target_link_libraries(MPDR randomprime ws2_32 userenv ${wxWidgets_LIBRARIES})
target_include_directories(MPDR PUBLIC "${CMAKE_SOURCE_DIR}/include/rapidjson/include" PRIVATE ${MPDR_DIRS})
