#ifndef STEREO_CAMERA_CALIBRATION_CHARUCO_BOARD_DETECTOR_H
#define STEREO_CAMERA_CALIBRATION_CHARUCO_BOARD_DETECTOR_H

#include <opencv2/objdetect/charuco_detector.hpp>

namespace YACC {
    class utility {
    public:
        static bool createDirs(const std::string &path);

        static bool findShowBoard(const cv::aruco::CharucoDetector& charuco, const cv::Mat& gray, const cv::Mat& frame);
    };
}

#endif //STEREO_CAMERA_CALIBRATION_CHARUCO_BOARD_DETECTOR_H