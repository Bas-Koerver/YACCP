#include <metavision/sdk/core/utils/frame_composer.h>
#include <opencv2/core/types.hpp>

#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

#include "recorders/prophesee_dvs_recorder.h"
#include "utility.h"
#include "recorders/basler_rgb_recorder.h"


int main() {
    (void) YACC::utility::createDirs("./data/images/");
    int squaresX{7};
    int squaresY{5};
    float squareLength{0.0295};
    float markerLength{0.0206};
    bool refine{false};
    int fps{30};
    int acc{33333};
    int numCams{2};
    std::vector<std::thread> threads;

    cv::aruco::DetectorParameters detParams;
    cv::aruco::CharucoParameters charucoParams;
    if (refine) {
        charucoParams.tryRefineMarkers = true;
    }

    cv::aruco::Dictionary dictionary{cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_50)};
    cv::aruco::CharucoBoard board{cv::Size(squaresX, squaresY), squareLength, markerLength, dictionary};
    cv::aruco::CharucoDetector charucoDetector(board, charucoParams, detParams);

    YACC::PropheseeDVSWorker propheseeDVSWorker(fps, acc, charucoDetector);
    // propheseeDVSWorker.listAvailableSources();

    YACC::BaslerRGBWorker baslerRGBWorker(fps, board, charucoDetector);

    threads.push_back(std::thread(&YACC::PropheseeDVSWorker::start, &propheseeDVSWorker));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    threads.push_back(std::thread(&YACC::BaslerRGBWorker::start, &baslerRGBWorker));

    for (auto &thread : threads) {
        thread.join();
    }

    return 0;

}
