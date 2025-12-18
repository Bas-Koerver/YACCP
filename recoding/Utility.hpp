#ifndef STEREO_CAMERA_CALIBRATION_CHARUCO_BOARD_DETECTOR_H
#define STEREO_CAMERA_CALIBRATION_CHARUCO_BOARD_DETECTOR_H

#include <opencv2/objdetect/charuco_detector.hpp>

namespace YACCP {
    struct CharucoResults {
        bool boardFound = false;
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f> > markerCorners;
        std::vector<int> charucoIds;
        std::vector<cv::Point2f> charucoCorners;
    };

    class Utility {
    public:
        static bool createDirs(const std::string &path);

        template<typename T>
        static std::vector<T> intersection(std::vector<T> vec1, std::vector<T> vec2);

        static CharucoResults findBoard(const cv::aruco::CharucoDetector &charucoDector,
                                        cv::Mat &gray,
                                        int cornerMin = 3);
    };

    template<typename T>
    std::vector<T> Utility::intersection(std::vector<T> vec1, std::vector<T> vec2) {
        std::vector<T> vecOutput;

        std::sort(vec1.begin(), vec1.end());
        std::sort(vec2.begin(), vec2.end());

        std::set_intersection(vec1.begin(), vec1.end(),
                              vec2.begin(), vec2.end(),
                              std::back_inserter(vecOutput));

        return vecOutput;
    }
}

#endif //STEREO_CAMERA_CALIBRATION_CHARUCO_BOARD_DETECTOR_H
