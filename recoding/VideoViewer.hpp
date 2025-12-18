#ifndef STEREO_CAMERA_CALIBRATION_VIDEO_VIEWER_H
#define STEREO_CAMERA_CALIBRATION_VIDEO_VIEWER_H
#include <stop_token>
#include <vector>
#include <opencv2/objdetect/charuco_detector.hpp>
#include <metavision/sdk/core/utils/frame_composer.h>

#include "recorders/camera_worker.hpp"


namespace YACCP {
    struct BoundingBoxData {
        int id;
        int camId;
        std::vector<int> charucoIds;
        std::vector<cv::Point2f> charucoCorners;
    };

    class VideoViewer {
    public:
        VideoViewer(std::stop_source stopSource,
                    int viewsHorizontal,
                    std::vector<CamData> &camDatas,
                    const cv::aruco::CharucoDetector &charucoDetector,
                    moodycamel::ReaderWriterQueue<BoundingBoxData> &boundingBoxQ);

        void start();

    private:
        std::stop_source stopSource_;
        std::stop_token stopToken_;
        int viewsHorizontal_ = 2;
        Metavision::FrameComposer frame_composer_;
        std::vector<CamData> &camDatas_;
        const cv::aruco::CharucoDetector charucoDetector_;
        moodycamel::ReaderWriterQueue<BoundingBoxData> &boundingBoxQ_;

        void processFrame(std::stop_token stopToken, CamData &camData, int camRef, std::atomic<int> &camDetectMode);

        std::tuple<std::vector<int>, std::vector<int> > calculateBiggestDims() const;

        std::tuple<int, int> calculateRowColumnIndex(int camIndex) const;

        std::vector<cv::Point> correctCordinates(const BoundingBoxData &boundingBoxData);

    };
} // YACCP

#endif //STEREO_CAMERA_CALIBRATION_VIDEO_VIEWER_H