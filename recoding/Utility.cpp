#include <filesystem>
#include <opencv2/core/mat.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

#include "Utility.hpp"

namespace YACCP {
    bool Utility::createDirs(const std::string &path) {
        std::filesystem::path p(path);
        return std::filesystem::create_directories(p);
    }

    // MISRA deviation: OpenCV Charuco API requires std::vector
    CharucoResults Utility::findBoard(const cv::aruco::CharucoDetector &charucoDector,
                                      cv::Mat &gray,
                                      int cornerMin) {
        CharucoResults charucoResults;

        charucoDector.detectBoard(gray, charucoResults.charucoCorners, charucoResults.charucoIds,
                                  charucoResults.markerCorners,
                                  charucoResults.markerIds);

        if (charucoResults.charucoCorners.size() > cornerMin) {
            charucoResults.boardFound = true;
        }
        return charucoResults;
    }
} // namespace YACCP
