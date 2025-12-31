#ifndef STEREO_CAMERA_CALIBRATION_CREATE_CHARUCO_BOARD_HPP
#define STEREO_CAMERA_CALIBRATION_CREATE_CHARUCO_BOARD_HPP
#include <opencv2/core.hpp>

namespace YACCP {
    class CreateCharucoBoard {
        public:
        static void createBoard(int squaresX, int squaresY, int squareLength, int markerLength, int border, int borderPoints,
                            cv::aruco::Dictionary& dictionary, bool generateImage, bool generateVideo, std::filesystem::path jobPath);

        private:
        static void createVideo(const cv::Mat& image, cv::Size size, std::filesystem::path jobPath);
    };
}

#endif //STEREO_CAMERA_CALIBRATION_CREATE_CHARUCO_BOARD_HPP