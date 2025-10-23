cmake_minimum_required(VERSION 4.0)
project(stereo-camera_calibration)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(camera_calibration main.cpp)
