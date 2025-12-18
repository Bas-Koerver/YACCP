
#ifndef STEREO_CAMERA_CALIBRATION_DETECTION_VALIDATOR_H
#define STEREO_CAMERA_CALIBRATION_DETECTION_VALIDATOR_H
#include <readerwriterqueue.h>

#include <opencv2/objdetect/charuco_detector.hpp>

#include "VideoViewer.hpp"

#include "recorders/camera_worker.hpp"

namespace YACCP {
    struct VerifyTask {
        int id;
        cv::Mat frame;
    };

    class DetectionValidator {
    public:
        DetectionValidator(std::stop_source stopSource,
                           std::vector<CamData> &camDatas,
                           const cv::aruco::CharucoDetector &charucoDetector,
                           moodycamel::ReaderWriterQueue<BoundingBoxData> &boundingBoxQ);


        void start();

    private:
        std::stop_source stopSource_;
        std::stop_token stopToken_;
        float intersectionMin_{.5F};
        std::vector<CamData> &camDatas_;
        const cv::aruco::CharucoDetector charucoDetector_;
        moodycamel::ReaderWriterQueue<BoundingBoxData> &boundingBoxQ_;
    };
} // YACCP

#endif //STEREO_CAMERA_CALIBRATION_DETECTION_VALIDATOR_H
