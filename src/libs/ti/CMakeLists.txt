project(ti)
cmake_minimum_required(VERSION 3.6)

find_package(catkin REQUIRED)
catkin_package(
        INCLUDE_DIRS ./
        LIBRARIES driverlib sensorlib utils utils_buffered
)

set(BUILD_TOOLS_DIR ../../../build-tools)

include(../../../build-tools/build_env.cmake)

FILE(GLOB driverlib_files driverlib/*.c driverlib/*.s)
message(STATUS "aaa ${driverlib_files}")
add_library(driverlib STATIC ${driverlib_files})
target_include_directories(driverlib PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
)

FILE(GLOB sensorlib_files sensorlib/*.c sensorlib/*.s)
add_library(sensorlib STATIC ${sensorlib_files})
target_include_directories(sensorlib PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
)

FILE(GLOB utils_files utils/*.c utils/*.s)
add_library(utils STATIC ${utils_files})
target_include_directories(utils PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
)

# buffered version of the utils file
add_library(utils_buffered STATIC ${utils_files})
target_include_directories(utils_buffered PUBLIC
        ${CMAKE_INCLUDE_PATH}
        ./
)
set_target_properties(utils_buffered PROPERTIES COMPILE_FLAGS "-DUART_BUFFERED")

# TODO: usblib