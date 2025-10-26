cmake_minimum_required(VERSION 4.0)
project(stereo-camera-calibration)

set(CMAKE_CXX_STANDARD 23)

find_package(OpenCV 4.11 REQUIRED COMPONENTS aruco)

#include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(stereo-camera-calibration main.cpp)

#target_link_libraries(stereo-camera-calibration PRIVATE
#        opencv_core
#        opencv_imgproc
#        opencv_highgui
#        opencv_imgcodecs
#        opencv_calib3d
#        opencv_aruco
#)
