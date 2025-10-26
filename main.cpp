#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>


static inline void createBoard()
{
    int squaresX = 5;
    int squaresY = 7;
    float squareLength = 0.04f; // in meters
    float markerLength = 0.02f; // in meters
    int margins = squareLength - markerLength;

    int borderBits = 1;

    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);
    cv::aruco::CharucoBoard board(cv::Size(squaresX, squaresY), squareLength, markerLength, dictionary);

    cv::Mat boardImage;
    cv::Size imageSize;
    imageSize.width = squaresX * squareLength + 2 * margins;
    imageSize.height = squaresY * squareLength + 2 * margins;
    board.generateImage(imageSize, boardImage, margins, borderBits);

    cv::imwrite("board.png", boardImage);
}

int main() {
    createBoard();
}