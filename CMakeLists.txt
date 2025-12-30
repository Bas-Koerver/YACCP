cmake_minimum_required(VERSION 3.23...3.27)
project(stereo-camera-calibration
        LANGUAGES CXX
        VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

find_package(MetavisionSDK COMPONENTS core stream ui REQUIRED)
find_package(pylon REQUIRED)

find_package(CLI11 CONFIG REQUIRED)

find_package(nlohmann_json REQUIRED)

find_package(OpenCV REQUIRED)

find_package(tomlplusplus CONFIG REQUIRED)

find_package(Boost COMPONENTS program_options REQUIRED)
add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS) ## needed to get rid of warning `#pragma message: The practice of declaring the Bind placeholders (_1, _2, ...)`


include(FetchContent)
FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
        GIT_TAG master
)
FetchContent_MakeAvailable(readerwriterqueue)

include_directories(${OpenCV_INCLUDE_DIRS})


add_executable(
        Calibration

        src/manager.cpp

        src/recoding/recorders/camera_worker.cpp src/recoding/recorders/camera_worker.hpp
        src/recoding/recorders/prophesee_cam_worker.cpp src/recoding/recorders/prophesee_cam_worker.hpp
        src/recoding/recorders/basler_cam_worker.cpp src/recoding/recorders/basler_cam_worker.hpp

        src/recoding/detection_validator.cpp src/recoding/detection_validator.hpp
        src/recoding/video_viewer.cpp src/recoding/video_viewer.hpp
        src/recoding/job_data.hpp

        src/tools/create_charuco_board.cpp src/tools/create_charuco_board.hpp
        src/tools/image_validator.cpp src/tools/image_validator.hpp

        src/utility.cpp src/utility.hpp
        src/calibration.cpp src/calibration.hpp
)

target_link_libraries(
        Calibration
        PRIVATE
        ${OpenCV_LIBS}
        MetavisionSDK::core MetavisionSDK::stream MetavisionSDK::ui
        pylon::pylon
        nlohmann_json::nlohmann_json
        tomlplusplus::tomlplusplus
        CLI11::CLI11
        PUBLIC
        readerwriterqueue
)