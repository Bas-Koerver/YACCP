#define NOMINMAX
#include <csignal>
#include <fstream>
#include <map>
#include <CLI/App.hpp>

#include "utility.hpp"
#include "recoding/detection_validator.hpp"
#include "recoding/video_viewer.hpp"

#include "recoding/recorders/basler_cam_worker.hpp"
#include "recoding/recorders/camera_worker.hpp"
#include "recoding/recorders/prophesee_cam_worker.hpp"

#include <GLFW/glfw3.h>

#include "tools/create_charuco_board.hpp"
#ifdef NDEBUG
#include <opencv2/core/utils/logger.hpp>
#endif

#include "calibration.hpp"
#include "recoding/job_data.hpp"
#include "tools/image_validator.hpp"
#include <toml++/toml.hpp>

int getWorstStopCode(const std::vector<YACCP::CamData> &camDatas) {
    int stopCode = 0;
    for (auto &camData: camDatas) {
        if (camData.runtimeData.exitCode > stopCode) {
            stopCode = camData.runtimeData.exitCode;
        }
    }

    return stopCode;
}

int main(int argc, char **argv) {
#ifdef NDEBUG
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_WARNING);
#endif

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        raise(SIGABRT);
    }
    auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

    CLI::App app{"Yet another camera calibration program"};
    app.get_formatter()->right_column_width(60);
    app.get_formatter()->column_width(60);
    app.set_help_all_flag("--help-all", "Expand all help");

    CLI::App *boardCreation = app.add_subcommand("create-board", "Board creation");
    CLI::App *record = app.add_subcommand("record", "Record detections");
    CLI::App *validate = app.add_subcommand("validate", "Validate recorded images");
    CLI::App *monoCalibration = app.add_subcommand("mono-calibration", "Perform mono calibration");
    CLI::App *stereoCalibration = app.add_subcommand("stereo-calibration", "Perform stereo calibration");
    app.require_subcommand(1);

    std::filesystem::path userPath;
    app.add_option(
        "-p,--path",
        userPath,
        "Path where config file is stored and where data directory will be placed"
    )->default_str("Defaults to current dir")->check(CLI::ExistingPath);

    auto squareLength{100};
    boardCreation->add_option(
        "-s, --square-length",
        squareLength,
        "The square length in pixels"
    )->default_val(100)->check(CLI::PositiveNumber);

    auto markerLength{70};
    boardCreation->add_option(
        "-m, --marker-length",
        markerLength,
        "The ArUco marker length in pixels"
    )->default_val(70)->check(CLI::PositiveNumber);

    int border{squareLength - markerLength};
    boardCreation->add_option(
        "-e, --marker-border",
        border,
        "The border size (margins) of the ArUco marker in pixels"
    )->default_str("square-length - marker-length");

    auto borderPoints{1};
    boardCreation->add_option(
        "-b, --border-point",
        borderPoints,
        "The amount of points (pixels) for the border"
    )->default_val(1)->check(CLI::PositiveNumber);

    auto generateImage{true};
    boardCreation->add_flag(
        "!-i, !--image",
        generateImage,
        "Whether to generate an image of the generated board"
    )->default_str("true");

    auto generateVideo{false};
    boardCreation->add_flag(
        "-v, --video",
        generateVideo,
        "Whether to generate an event video of the generated board"
    )->default_str("false");

    CLI11_PARSE(app, argc, argv);

    auto now = std::chrono::system_clock::now();
    auto localTime = std::chrono::system_clock::to_time_t(now);
    std::stringstream dateTime;
    dateTime << std::put_time(std::localtime(&localTime), "%F_%H-%M-%S");

    auto workingDir = std::filesystem::current_path();
    std::filesystem::path path = workingDir / userPath;
    std::filesystem::path jobPath = path / "data" / ("job_" + dateTime.str());
    std::filesystem::create_directories(jobPath);


    // TODO: Catch invalid paths.
    // std::filesystem::path path = workingDir / userPath / "data";


    toml::table tbl;
    try
    {
        tbl = toml::parse_file((path / "config.toml").string());
        std::cout << tbl << "\n";
    }
    catch (const toml::parse_error& err)
    {
        std::cerr << "Parsing failed:\n" << err << "\n";
        return 1;
    }

    auto squaresX{7};
    auto squaresY{5};
    auto dictId{8};

    if (tbl["board"]["squares_x"]) squaresX = tbl["board"]["squares_x"].value<int>().value();
    if (tbl["board"]["squares_y"]) squaresY = tbl["board"]["squares_y"].value<int>().value();
    if (tbl["detection"]["charuco_dictionary"]) squaresX = tbl["detection"]["charuco_dictionary"].value<int>().value();
    cv::aruco::Dictionary dictionary{cv::aruco::getPredefinedDictionary(dictId)};

    if (*boardCreation) YACCP::CreateCharucoBoard::createBoard(squaresX, squaresY, squareLength, markerLength, border, borderPoints, dictionary, generateImage, generateVideo, jobPath);

    std::cout << std::filesystem::absolute(userPath) << "\n";

    // std::cout << "Working on --file from start: " << files << '\n';
    // std::cout << "Working on --count from stop: " << s->count() << ", direct count: " << boardCreation->count("--count")
    //         << '\n';
    // std::cout << "Count of --random flag: " << app.count("--random") << '\n';
    // for (auto *subcom: app.get_subcommands())
    //     std::cout << "Subcommand: " << subcom->get_name() << '\n';

    return 0;


    // TODO: Add config / CLI
    // CLI to define high level parameters, what to do (create board, record, validate images, calibrate), where
    // config file is saved.
    // config files to define low level parameters, camera types, ids, fps, accumulation time, board parameters etc.
    std::vector<YACCP::WorkerTypes> slaveWorkers;
    YACCP::WorkerTypes masterWorker;
    slaveWorkers = {YACCP::WorkerTypes::prophesee};
    masterWorker = YACCP::WorkerTypes::basler;
    std::vector camPlacement{0, 1};

    auto refine{false};
    auto fps{30};
    auto acc{33333};
    auto viewsHorizontal{3};
    float cornerMin{.25F};
    // cv::aruco::DICT_6X6_50


    // Current working directory.
    // auto workingDir = std::filesystem::current_path();
    std::stop_source stopSource;
    auto numCams{static_cast<int>(slaveWorkers.size() + 1)};
    std::vector<YACCP::CamData> camDatas(numCams);
    std::vector<std::jthread> threads;
    std::vector<std::unique_ptr<YACCP::CameraWorker> > cameraWorkers;
    moodycamel::ReaderWriterQueue<YACCP::ValidatedCornersData> valCornersQ{100};
    cv::aruco::CharucoParameters charucoParams;
    cv::aruco::DetectorParameters detParams;
    std::map<std::string, YACCP::WorkerTypes>{
        {"PropheseeDVS", YACCP::WorkerTypes::prophesee},
        {"BaslerRGB", YACCP::WorkerTypes::basler},
    };

    if (refine) {
        charucoParams.tryRefineMarkers = true;
    }

    dateTime << std::put_time(std::localtime(&localTime), "%F_%H-%M-%S");
    // TODO: Catch invalid paths.
    // std::filesystem::path path = workingDir / userPath / "data";
    std::filesystem::path outputPath = path / ("job_" + dateTime.str() + "/");

    // cv::aruco::Dictionary dictionary{cv::aruco::getPredefinedDictionary(dictId)};
    cv::aruco::CharucoBoard board{
        cv::Size(squaresX, squaresY),
        static_cast<float>(squareLength),
        static_cast<float>(markerLength),
        dictionary
    };
    cv::aruco::CharucoDetector charucoDetector(board, charucoParams, detParams);

    YACCP::ImageValidator imageValidator(mode->width - 100,
                                         mode->height - 100,
                                         path);
    imageValidator.listJobs();
    // imageValidator.validateImages(
    //     "job_2025-12-28_18-54-04"
    // );

    YACCP::Calibration calibration(charucoDetector, path, cornerMin);

    calibration.monoCalibrate("job_2025-12-28_18-54-04");

    return 0;

    (void) std::filesystem::create_directories(outputPath / "images/raw");
    // (void) std::filesystem::create_directories(outputPath / "images/verified");

    for (int i = 0; i < numCams - 1; ++i) {
        camDatas[i].info.isMaster = false;
        camDatas[i].info.camId = i;

        switch (slaveWorkers[i]) {
            case YACCP::WorkerTypes::prophesee:
                cameraWorkers.emplace_back(std::make_unique<YACCP::PropheseeCamWorker>(
                    stopSource,
                    camDatas,
                    fps,
                    i,
                    acc,
                    1,
                    outputPath
                ));
                break;

            case YACCP::WorkerTypes::basler:
                cameraWorkers.emplace_back(std::make_unique<YACCP::BaslerCamWorker>(
                    stopSource,
                    camDatas,
                    fps,
                    i,
                    outputPath
                ));
                break;
        }

        auto *worker = cameraWorkers.back().get();
        threads.emplace_back([worker] { worker->start(); });

        while (!camDatas[i].runtimeData.isRunning) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (stopSource.stop_requested()) {
                return getWorstStopCode(camDatas);
            }
        }
    }

    camDatas.back().info.isMaster = true;
    camDatas.back().info.camId = camDatas.size() - 1;
    switch (masterWorker) {
        case YACCP::WorkerTypes::prophesee:
            cameraWorkers.emplace_back(std::make_unique<YACCP::PropheseeCamWorker>(
                stopSource,
                camDatas,
                fps,
                camDatas.size() - 1,
                acc,
                1,
                outputPath
            ));
            break;

        case YACCP::WorkerTypes::basler:
            cameraWorkers.emplace_back(std::make_unique<YACCP::BaslerCamWorker>(
                stopSource,
                camDatas,
                fps,
                camDatas.size() - 1,
                outputPath
            ));
            break;
    }

    auto *worker = cameraWorkers.back().get();
    threads.emplace_back([worker] { worker->start(); });

    // Start the viewing thread.
    while (!camDatas.back().runtimeData.isRunning) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (stopSource.stop_requested()) {
            return getWorstStopCode(camDatas);
        }
    }

    YACCP::VideoViewer videoViewer{
        stopSource,
        viewsHorizontal,
        mode->width - 100,
        mode->height - 100,
        camDatas,
        charucoDetector,
        valCornersQ,
        outputPath,
        cornerMin
    };
    threads.emplace_back(&YACCP::VideoViewer::start, &videoViewer);

    YACCP::DetectionValidator detectionValidator{
        stopSource,
        camDatas,
        charucoDetector,
        valCornersQ,
        outputPath,
        cornerMin
    };
    threads.emplace_back(&YACCP::DetectionValidator::start, &detectionValidator);

    for (auto &thread: threads) {
        thread.join();
    }

    // Create json object with camera data.
    std::cout << "Creating job_data.json";
    YACCP::CharucoConfig charucoConfig;
    charucoConfig.openCvDictionaryId = dictId;
    charucoConfig.boardSize = cv::Size(squaresX, squaresY);
    charucoConfig.squareLength = squareLength;
    charucoConfig.markerLength = markerLength;
    charucoConfig.charucoParams = charucoParams;
    charucoConfig.detParams = detParams;
    nlohmann::json j;
    j["openCv"] = CV_VERSION;
    j["camPlacement"] = camPlacement;
    j["charuco"] = charucoConfig;
    j["cams"] = nlohmann::json::object();

    for (auto i{0}; i < camDatas.size(); ++i) {
        j["cams"]["cam_" + std::to_string(i)] = camDatas[i].info;
    }

    // Save json to file.
    std::ofstream file(outputPath / "job_data.json");
    file << j.dump(4);

    return 0;
}
