cmake_minimum_required(VERSION 3.0.0)

project(CanDevStudio CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

option(WITH_COVERAGE OFF)

find_package(Qt5Core REQUIRED)
find_package(Qt5SerialBus REQUIRED)
find_package(Qt5Widgets REQUIRED)

if(Qt5Core_VERSION LESS 5.8)
    message(SEND_ERROR "ERROR: Detected Qt verison: ${Qt5Widgets_VERSION}. Minimum Qt version: 5.8.0")
endif()

if(NOT MSVC)
    #MSVC produces a lot of errors due to Qt headers
    add_definitions(-Wall -Wextra)
endif()

if(WITH_COVERAGE)
    set(CMAKE_CXX_FLAGS "-g -O0 -Wwrite-strings -fprofile-arcs -ftest-coverage")
    set(CMAKE_C_FLAGS="-g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
    set(CMAKE_EXE_LINKER_FLAGS="-fprofile-arcs -ftest-coverage")
endif()

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src/gui)
add_subdirectory(src/components)

enable_testing()
add_subdirectory(3rdParty/fakeit)
add_subdirectory(tests)
