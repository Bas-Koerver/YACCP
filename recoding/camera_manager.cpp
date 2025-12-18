#include <map>

#include "detection_validator.hpp"
#include "Utility.hpp"
#include "VideoViewer.hpp"

#include "recorders/basler_rgb_recorder.hpp"
#include "recorders/camera_worker.hpp"
#include "recorders/prophesee_dvs_recorder.hpp"

int getWorstStopCode(const std::vector<YACCP::CamData> &camDatas) {
    int stopCode = 0;
    for (auto &camData: camDatas) {
        if (camData.exitCode > stopCode) {
            stopCode = camData.exitCode;
        }
    }

    return stopCode;
}

int main() {
    (void) YACCP::Utility::createDirs("./data/images/");
    std::map<std::string, YACCP::WorkerTypes>{
        {"PropheseeDVS", YACCP::WorkerTypes::propheseeDvsEvk4},
        {"BaslerRGB", YACCP::WorkerTypes::baslerRgb},
    };

    // TODO: Add config / CLI
    std::vector<YACCP::WorkerTypes> slaveWorkers;
    YACCP::WorkerTypes masterWorker;
    // slaveWorkers = {YACCP::WorkerTypes::propheseeDvsEvk4};
    masterWorker = YACCP::WorkerTypes::baslerRgb;
    auto squaresX{7};
    auto squaresY{5};
    float squareLength{0.0295};
    float markerLength{0.0206};
    auto refine{false};
    auto fps{30};
    auto acc{33333};
    auto viewsHorizontal{3};

    std::stop_source stopSource;
    auto numCams{static_cast<int>(slaveWorkers.size() + 1)};
    std::vector<YACCP::CamData> camDatas(numCams);
    std::vector<std::jthread> threads;
    std::vector<std::unique_ptr<YACCP::CameraWorker> > cameraWorkers;
    moodycamel::ReaderWriterQueue<YACCP::BoundingBoxData> boundingBoxQ{100};
    cv::aruco::DetectorParameters detParams;
    cv::aruco::CharucoParameters charucoParams;
    if (refine) {
        charucoParams.tryRefineMarkers = true;
    }

    cv::aruco::Dictionary dictionary{cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50)};
    cv::aruco::CharucoBoard board{cv::Size(squaresX, squaresY), squareLength, markerLength, dictionary};
    cv::aruco::CharucoDetector charucoDetector(board, charucoParams, detParams);

    for (int i = 0; i < numCams - 1; i++) {
        camDatas[i].isMaster = false;

        switch (slaveWorkers[i]) {
            case YACCP::WorkerTypes::propheseeDvsEvk4:
                cameraWorkers.emplace_back(std::make_unique<YACCP::PropheseeDVSWorker>(
                    stopSource,
                    camDatas,
                    fps,
                    i,
                    acc,
                    1
                ));
                break;

            case YACCP::WorkerTypes::baslerRgb:
                cameraWorkers.emplace_back(std::make_unique<YACCP::BaslerRGBWorker>(
                    stopSource,
                    camDatas,
                    fps,
                    i
                ));
                break;
        }

        auto *worker = cameraWorkers.back().get();
        threads.emplace_back([worker] { worker->start(); });

        while (!camDatas[i].isRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (stopSource.stop_requested()) {
                return getWorstStopCode(camDatas);
            }
        }
    }

    camDatas.back().isMaster = true;
    switch (masterWorker) {
        case YACCP::WorkerTypes::propheseeDvsEvk4:
            cameraWorkers.emplace_back(std::make_unique<YACCP::PropheseeDVSWorker>(
                stopSource,
                camDatas,
                fps,
                camDatas.size() - 1,
                acc,
                1
            ));
            break;

        case YACCP::WorkerTypes::baslerRgb:
            cameraWorkers.emplace_back(std::make_unique<YACCP::BaslerRGBWorker>(
                stopSource,
                camDatas,
                fps,
                camDatas.size() - 1
            ));
            break;
    }

    auto *worker = cameraWorkers.back().get();
    threads.emplace_back([worker] { worker->start(); });

    // Start the viewing thread.
    while (!camDatas.back().isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (stopSource.stop_requested()) {
            return getWorstStopCode(camDatas);
        }
    }

    YACCP::VideoViewer videoViewer{stopSource, viewsHorizontal, camDatas, charucoDetector, boundingBoxQ};
    threads.emplace_back(&YACCP::VideoViewer::start, &videoViewer);

    YACCP::DetectionValidator detectionValidator{stopSource, camDatas, charucoDetector, boundingBoxQ};
    threads.emplace_back(&YACCP::DetectionValidator::start, &detectionValidator);

    for (auto &thread: threads) {
        thread.join();
    }

    return 0;
}
