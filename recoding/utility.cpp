#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

#include "utility.h"

namespace YACC {
    bool utility::createDirs(const std::string &path) {
        std::filesystem::path p(path);
        return std::filesystem::create_directories(p);
    }

    // MISRA deviation: OpenCV Charuco API requires std::vector
    bool utility::findShowBoard(const cv::aruco::CharucoDetector &charuco, const cv::Mat &gray,
                                               const cv::Mat &frame) {
        bool boardFound = false;
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f> > markerCorners;
        std::vector<int> charucoIds;
        std::vector<cv::Point2f> charucoCorners;

        charuco.detectBoard(gray, charucoCorners, charucoIds, markerCorners, markerIds);


        if (!markerIds.empty())
            cv::aruco::drawDetectedMarkers(frame, markerCorners, markerIds);
        if (!charucoIds.empty())
            cv::aruco::drawDetectedCornersCharuco(frame, charucoCorners, charucoIds, cv::Scalar(0, 255, 0));

        if (charucoCorners.size() > 3) {
            boardFound = true;
        }
        return boardFound;
    }
}