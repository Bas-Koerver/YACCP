#include "calibration.hpp"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <nlohmann/json_fwd.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgcodecs.hpp>

#include "utility.hpp"

namespace YACCP {
    Calibration::Calibration(
        const cv::aruco::CharucoDetector &charucoDetector,
        const std::filesystem::path &dataPath,
        float cornerMin)
        : charucoDetector_(charucoDetector),
          dataPath_(dataPath),
          cornerMin_(cornerMin) {
    }

    // void Calibration::processImage(std::vector<cv::Point3f> objPoints, std::vector<cv::Point2f> imagePoints) {
    //
    // }

    void Calibration::calibrate(const std::string &jobName) {
        jobPath_ = dataPath_ / jobName;
        if (!exists(jobPath_)) {
            std::cerr << "Job: " << jobName << " does not exist in the given path: " << dataPath_ << "\n";
            return;
        }

        std::vector<std::filesystem::path> cams;
        std::vector<std::filesystem::path> files;
        std::vector<std::vector<cv::Point3f> > allObjPoints;
        std::vector<std::vector<cv::Point2f> > allImgPoints;

        for (auto const &entry: std::filesystem::directory_iterator(jobPath_ / "images/raw")) {
            if (!entry.is_directory()) continue;
            cams.push_back(entry.path().filename());
        }
        for (auto const &entry: std::filesystem::directory_iterator(jobPath_ / "images/raw" / cams[0])) {
            if (!entry.is_regular_file()) continue;
            files.push_back(entry.path().filename());
        }

        if (!exists(jobPath_ / "camera_data.json")) {
            std::cerr << "No camera data was found.\n Stopping! \n";
            return;
        }

        std::ifstream file{jobPath_ / "camera_data.json"};
        nlohmann::json jsonObj{nlohmann::json::parse(file)};

        for (auto &cam: cams) {
            cv::Mat cameraMatrix;
            cv::Mat distCoeffs;
            std::vector<cv::Mat> rvecs;
            std::vector<cv::Mat> tvecs;


            for (auto &file: files) {
                std::vector<cv::Point3f> objPoints;
                std::vector<cv::Point2f> imgPoints;

                cv::Mat img{cv::imread((jobPath_ / "images/verified" / cam / file).string(), cv::IMREAD_GRAYSCALE)};

                auto results{Utility::findBoard(charucoDetector_, img, cornerMin_)};
                if (!results.boardFound) continue;

                charucoDetector_.getBoard().matchImagePoints(results.charucoCorners, results.charucoIds, objPoints,
                                                             imgPoints);
                allObjPoints.push_back(objPoints);
                allImgPoints.push_back(imgPoints);
            }

            // cv::calibrateCamera(allObjPoints,
            //                     allImgPoints,
            //                     cv::Size(jsonObj[cam]["width"], jsonObj[cam]["height"]),
            //                     cameraMatrix,
            //                     distCoeffs,
            //                     rvecs,
            //                     tvecs);
        }
    }
}
