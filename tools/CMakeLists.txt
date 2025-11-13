#find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui videoio aruco)
find_package(OpenCV COMPONENTS core highgui imgproc videoio imgcodecs calib3d objdetect aruco REQUIRED)

find_package(CLI11 REQUIRED)

add_executable(create-charuco-board create_charuco_board.cpp)

target_link_libraries(create-charuco-board ${OpenCV_LIBS} CLI11::CLI11)