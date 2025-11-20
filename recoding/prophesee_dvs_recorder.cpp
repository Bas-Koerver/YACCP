#include <metavision/sdk/core/algorithms/periodic_frame_generation_algorithm.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>
#include <metavision/sdk/ui/utils/window.h>
#include <metavision/sdk/ui/utils/mt_window.h>
#include <metavision/sdk/ui/utils/event_loop.h>
#include <metavision/sdk/stream/camera.h>
#include <metavision/sdk/base/events/event_cd.h>

#include <metavision/hal/device/device_discovery.h>
#include <metavision/hal/device/device.h>
#include <metavision/hal/facilities/i_trigger_in.h>
#include <metavision/hal/facilities/i_ll_biases.h>

#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

#include "utility.h"


int main() {
    int exitCode = 0;

    // ChArUco detector params
    int squaresX = 7;
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
    Metavision::Camera cam;

    bool camOpen = false;

    try {
        // open the first available camera
        cam = Metavision::Camera::from_first_available();
        camOpen = true;
    } catch (Metavision::CameraException& e) {
        std::cerr << e.what() << "\n";
        exitCode = 2;
    }
    if (camOpen) {
        // TODO: add serial handling.

        // TODO: add biases file loading.

        // TODO: add runtime error handling

        // Configure biases for less noise
        auto &device = cam.get_device();
        auto *biases = device.get_facility<::Metavision::I_LL_Biases>();
        (void)biases->set("bias_diff_off", 40);
        (void)biases->set("bias_diff_on", 40);

        // Enable trigger in port
        auto i_trigger_in = cam.get_device().get_facility<Metavision::I_TriggerIn>();
        (void)i_trigger_in->enable(Metavision::I_TriggerIn::Channel::Main);

        // TODO: change this to only save cam geometry as a reference.
        const auto &geometry = cam.geometry();


        std::mutex cd_frame_generator_mutex;
        Metavision::CDFrameGenerator cdFrameGenerator{geometry.get_width(), geometry.get_height()};
        cdFrameGenerator.set_display_accumulation_time_us(acc);

        std::mutex cd_frame_mutex;
        cv::Mat cd_frame;
        Metavision::timestamp cd_frame_ts{0};
        (void)cdFrameGenerator.start(
            fps, [&cd_frame_mutex, &cd_frame, &cd_frame_ts](const Metavision::timestamp &ts, const cv::Mat &frame) {
                std::unique_lock<std::mutex> lock(cd_frame_mutex);
                cd_frame_ts = ts;
                frame.copyTo(cd_frame);
            });

        Metavision::MTWindow window("MTWindow BGR", geometry.get_width(), geometry.get_height(),
                                     Metavision::Window::RenderMode::BGR);

        window.set_keyboard_callback(
            [&window](Metavision::UIKeyEvent key, int scancode, Metavision::UIAction action, int mods) {
                if (action == Metavision::UIAction::RELEASE && key == Metavision::UIKeyEvent::KEY_ESCAPE) {
                    window.set_close_flag();
                }
            });

        (void)cam.cd().add_callback(
            [&cd_frame_generator_mutex, &cdFrameGenerator
            ](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                // frame_gen.process_events(begin, end);
                std::unique_lock<std::mutex> lock(cd_frame_generator_mutex);
                cdFrameGenerator.add_events(begin, end);
            });


        (void)cam.start();
        (void)cam.start_recording("test.raw");
        while (cam.is_running() && !window.should_close()) {
            Metavision::EventLoop::poll_and_dispatch();

            if (!cd_frame.empty()) {
                cv::cvtColor(cd_frame, gray, cv::COLOR_BGR2GRAY);
                cv::Mat viz{cd_frame.clone()};
                utility::findShowBoard(charuco, gray, viz);
                window.show_async(viz);
            }
        }
        (void)cam.stop_recording("test.raw");
        (void)cam.stop();
    }
    return exitCode;
}
