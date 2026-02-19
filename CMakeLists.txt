cmake_minimum_required(VERSION 3.23...3.27)
project(YACCP
        LANGUAGES CXX
        VERSION 0.1)

set(CMAKE_CXX_STANDARD 23)

# Define which SDKs are available.
# If Metavision/OpenEB is not available recording and validation will be disabled.
option(YACCP_ENABLE_METAVISION "Enable Metavision SDK support" OFF)
option(YACCP_ENABLE_PYLON "Enable Basler pylon SDK support" OFF)

set(YACCP_HAS_METAVISION ${YACCP_ENABLE_METAVISION})
if (YACCP_ENABLE_METAVISION)
        find_package(MetavisionSDK 5.1.0 COMPONENTS core stream ui QUIET)
        find_package(MetavisionHAL 5.1.0 QUIET)
        if (NOT (MetavisionSDK_FOUND AND MetavisionHAL_FOUND))
                message(WARNING "Metavision SDK not found; building without Metavision camera worker support, disabling recording and validation.")
                set(YACCP_HAS_METAVISION OFF)
        endif()
endif()

set(YACCP_HAS_PYLON ${YACCP_ENABLE_PYLON})
if (YACCP_ENABLE_PYLON)
        find_package(pylon 10.3.2.636 QUIET)
        if (NOT pylon_FOUND)
                message(WARNING "pylon SDK not found; building without Basler camera worker support.")
                set(YACCP_HAS_PYLON OFF)
        endif()
endif()

# General packages
find_package(Boost 1.78.0 REQUIRED)
find_package(OpenCV 4.8.0 COMPONENTS core imgproc imgcodecs videoio objdetect REQUIRED)
find_package(glfw3 3.3 REQUIRED)

# CLI and config
find_package(nlohmann_json 3.12.0 REQUIRED)
find_package(tomlplusplus 3.4.0 CONFIG REQUIRED)
find_package(CLI11 2.6.1 CONFIG REQUIRED)

add_compile_definitions(BOOST_BIND_GLOBAL_PLACEHOLDERS) ## needed to get rid of warning `#pragma message: The practice of declaring the Bind placeholders (_1, _2, ...)`

include(FetchContent)
FetchContent_Declare(
        readerwriterqueue
        GIT_REPOSITORY https://github.com/cameron314/readerwriterqueue
        GIT_TAG master
)
FetchContent_MakeAvailable(readerwriterqueue)

include_directories(${OpenCV_INCLUDE_DIRS})

set(YACCP_SOURCES
        src/yaccp.cpp

        src/utility.cpp                                 src/utility.hpp
        src/camera_calibration.cpp                      src/camera_calibration.hpp

        src/cli/orchestrator.cpp                        src/cli/orchestrator.hpp
        src/cli/board_creation.cpp                      src/cli/board_creation.hpp
        src/cli/calibration.cpp                         src/cli/calibration.hpp
        src/cli/recording.cpp                           src/cli/recording.hpp
        src/cli/validation.cpp                          src/cli/validation.hpp

        src/config/orchestrator.cpp                     src/config/orchestrator.hpp
        src/config/board.cpp                            src/config/board.hpp
        src/config/detection.cpp                        src/config/detection.hpp
        src/config/recording.cpp                        src/config/recording.hpp
        src/config/viewing.cpp                          src/config/viewing.hpp

        src/executors/board_runner.cpp                  src/executors/board_runner.hpp
        src/executors/calibration_runner.cpp            src/executors/calibration_runner.hpp

                                                        src/global_variables/config_defaults.hpp
                                                        src/global_variables/cli_defaults.hpp
                                                        src/global_variables/program_defaults.hpp

                                                        src/recoding/job_data.hpp

        src/tools/create_board.cpp                      src/tools/create_board.hpp

        src/recoding/recorders/camera_worker.cpp        src/recoding/recorders/camera_worker.hpp
)

if (YACCP_HAS_METAVISION)
        list(APPEND YACCP_SOURCES
                src/executors/recording_runner.cpp              src/executors/recording_runner.hpp
                src/executors/validation_runner.cpp             src/executors/validation_runner.hpp

                src/recoding/detection_validator.cpp            src/recoding/detection_validator.hpp
                src/recoding/video_viewer.cpp                   src/recoding/video_viewer.hpp

                src/tools/image_validator.cpp                   src/tools/image_validator.hpp

                src/recoding/recorders/prophesee_cam_worker.cpp src/recoding/recorders/prophesee_cam_worker.hpp
        )
endif()

if (YACCP_HAS_PYLON)
        list(APPEND YACCP_SOURCES
                src/recoding/recorders/basler_cam_worker.cpp    src/recoding/recorders/basler_cam_worker.hpp
        )
endif()

add_executable(YACCP
        ${YACCP_SOURCES}
)

target_link_libraries(
        YACCP
        PRIVATE
        ${OpenCV_LIBS}
        nlohmann_json::nlohmann_json
        tomlplusplus::tomlplusplus
        CLI11::CLI11
        readerwriterqueue
        glfw
)

if (YACCP_HAS_METAVISION)
        target_link_libraries(
                YACCP
                PRIVATE
                MetavisionSDK::core MetavisionSDK::stream MetavisionSDK::ui
                Metavision::HAL
        )
endif()

if (YACCP_HAS_PYLON)
        target_link_libraries(
                YACCP
                PRIVATE
                pylon::pylon
        )
endif()

target_compile_definitions(
        YACCP
        PRIVATE
        YACCP_HAS_METAVISION=$<BOOL:${YACCP_HAS_METAVISION}>
        YACCP_HAS_PYLON=$<BOOL:${YACCP_HAS_PYLON}>
)

message(STATUS "YACCP Metavision support: ${YACCP_HAS_METAVISION}")
message(STATUS "YACCP Basler(pylon) support: ${YACCP_HAS_PYLON}")
