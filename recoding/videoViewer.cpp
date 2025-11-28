#include <metavision/sdk/core/utils/frame_composer.h>

#include "VideoViewer.h"

namespace YACC {
    VideoViewer::VideoViewer(unsigned width, unsigned height, cv::Mat &frame) : width_(width), height_(height),
        frame_(frame) {
    }

    void VideoViewer::start() {
        Metavision::FrameComposer frame_composer;
        const auto frame1{
            frame_composer.add_new_subimage_parameters(0, 0, {width_, height_},
                                                       Metavision::FrameComposer::GrayToColorOptions())
        };
        const auto right_image_ref = frame_composer.add_new_subimage_parameters(
            width_ + 10, 0, {width_, height_}, Metavision::FrameComposer::GrayToColorOptions());
    }
} // YACC
