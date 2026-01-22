#include "calibration_runner.hpp"

#include "../camera_calibration.hpp"
#include "../utility.hpp"


namespace YACCP::Executor {
    void runCalibration(CLI::CliCmdConfig& cliCmdConfig,
        CLI::CliCmds& cliCmds,
        std::filesystem::path path,
        const std::stringstream& dateTime) {

        const std::filesystem::path dataPath{path / "data"};
        std::filesystem::path jobPath = dataPath / cliCmdConfig.calibrationCmdConfig.jobId;

        Config::FileConfig fileConfig;
        // Load config from JSON file
        nlohmann::json j = Utility::loadJsonFromFile(jobPath, "job_data.json");
        j.at("config").get_to(fileConfig);

        // Variable setup based on config.
        cv::aruco::Dictionary dictionary{
            cv::aruco::getPredefinedDictionary(fileConfig.detectionConfig.openCvDictionaryId)
        };
        cv::aruco::CharucoParameters charucoParams;
        cv::aruco::DetectorParameters detParams;
        cv::aruco::CharucoBoard board{
            fileConfig.boardConfig.boardSize,
            fileConfig.boardConfig.squareLength,
            fileConfig.boardConfig.markerLength,
            dictionary
        };
        cv::aruco::CharucoDetector charucoDetector(board, charucoParams, detParams);

        CameraCalibration cameraCalibration(charucoDetector, dataPath, fileConfig.detectionConfig.cornerMin);

        if (*cliCmds.calibrationCmds.mono) {
            cameraCalibration.monoCalibrate(cliCmdConfig.calibrationCmdConfig.jobId);
        } else if (*cliCmds.calibrationCmds.stereo) {
            std::cout << "stereo calibration called\n";
        } else {
            std::cout << "base calibration called\n";
        }
    }
} // YACCP::Executor
