#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <opencv2/highgui.hpp>

#include <pylon/PylonIncludes.h>
#ifdef PYLON_WIN_BUILD
#    include <pylon/PylonGUI.h>
#endif

#include "utility.h"

int main() {
    int exitCode = 0;

    // ChArUco detector params
    int squaresX{7};
    int squaresY = 5;
    float squareLength = 0.0295;
    float markerLength = 0.0206;
    bool refine = false;
    const std::uint32_t acc = 10000;
    int fps = 30;

    cv::aruco::DetectorParameters detParams;
    cv::aruco::CharucoParameters charucoParams;
    if (refine) {
        charucoParams.tryRefineMarkers = true;
    }

    cv::aruco::Dictionary dictionary{cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50)};
    cv::aruco::CharucoBoard board{cv::Size(squaresX, squaresY), squareLength, markerLength, dictionary};
    cv::aruco::CharucoDetector charuco(board, charucoParams, detParams);

    cv::Mat gray;

    bool camOpen = false;

    Pylon::PylonInitialize();

    Pylon::CInstantCamera camera{Pylon::CTlFactory::GetInstance().CreateFirstDevice()};

    std::cout << "Using device: " << camera.GetDeviceInfo().GetModelName() << "\n";

    GenApi::INodeMap& nodemap = camera.GetNodeMap();
    Pylon::CEnumParameter pixelFormat( nodemap, "PixelFormat" );
    if (pixelFormat.CanSetValue("Mono8")) {
        pixelFormat.SetValue("Mono8");
    }

    camera.StartGrabbing();

    Pylon::CGrabResultPtr ptrGrabResult;

    while (camera.IsGrabbing()) {
        (void)camera.RetrieveResult(5000, ptrGrabResult, Pylon::TimeoutHandling_ThrowException);
        if (ptrGrabResult -> GrabSucceeded()) {
            cv::Mat image{static_cast<int>(ptrGrabResult->GetHeight()), static_cast<int>(ptrGrabResult->GetWidth()), CV_8UC1, ptrGrabResult->GetBuffer()};
            cv::Mat viz = image.clone();
            utility::findShowBoard(charuco, image, viz);
            cv::imshow("basler", viz);
            cv::waitKey(1);
        }

    }

}