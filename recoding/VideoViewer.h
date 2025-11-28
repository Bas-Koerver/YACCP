//
// Created by Bas_K on 2025-11-26.
//

#ifndef STEREO_CAMERA_CALIBRATION_VIDEO_VIEWER_H
#define STEREO_CAMERA_CALIBRATION_VIDEO_VIEWER_H

namespace YACC {
    class VideoViewer {
        public:
        VideoViewer(unsigned width, unsigned height, cv::Mat &frame);

        void start();

        private:
        const unsigned int width_;
        const unsigned int height_;
        cv::Mat& frame_;
    };
} // YACC

#endif //STEREO_CAMERA_CALIBRATION_VIDEO_VIEWER_H