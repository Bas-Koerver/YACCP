#include <metavision/sdk/stream/camera.h>
#include <metavision/sdk/core/algorithms/periodic_frame_generation_algorithm.h>
#include <metavision/sdk/ui/utils/window.h>
#include <metavision/sdk/ui/utils/mt_window.h>
#include <metavision/hal/facilities/i_trigger_in.h>
#include <metavision/hal/device/device.h>
#include <metavision/sdk/ui/utils/event_loop.h>
#include <metavision/hal/device/device_discovery.h>

#include <metavision/sdk/base/events/event_cd.h>
#include <metavision/hal/facilities/i_ll_biases.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>
#include <opencv2/highgui.hpp>

#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>


int main() {

    // ChArUco detector params
    int squaresX = 7;
    int squaresY = 5;
    float squareLength = 0.0295;
    float markerLength = 0.0206;
    bool refine = true;

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

    try {
        // open the first available cameraa
        cam = Metavision::Camera::from_first_available();
    } catch (const Metavision::CameraException &e) {
        std::cerr << e.what() << "\n";
        return 2;
    }

    // Configure biases for less noise
    auto &device = cam.get_device();
    auto *biases = device.get_facility<::Metavision::I_LL_Biases>();
    biases->set("bias_diff_off", 40);
    biases->set("bias_diff_on", 40);

    // Enable trigger in port
    auto i_trigger_in = cam.get_device().get_facility<Metavision::I_TriggerIn>();
    i_trigger_in->enable(Metavision::I_TriggerIn::Channel::Main);

    // cam.ext_trigger().add_callback([&](const Metavision::EventExtTrigger *begin,
    //                                    const Metavision::EventExtTrigger *end) {
    //     for (auto ev = begin; ev != end; ++ev)
    //         std::cout << "Trigger event at t=" << ev->t << " polarity=" << (int) ev->p << std::endl;
    // });

    const int w = cam.geometry().get_width();
    const int h = cam.geometry().get_height();

    const std::uint32_t acc = 10000;
    double fps = 30;

    std::mutex cd_frame_generator_mutex;
    auto frame_gen = Metavision::PeriodicFrameGenerationAlgorithm(w, h, acc, fps);
    // Metavision::CDFrameGenerator cdFrameGenerator(w, h);
    // cdFrameGenerator.set_display_accumulation_time_us(acc);

    // std::mutex cd_frame_mutex;
    // cv::Mat cd_frame;
    // Metavision::timestamp cd_frame_ts{0};
    // cdFrameGenerator.start(
    //     30, [&cd_frame_mutex, &cd_frame, &cd_frame_ts](const Metavision::timestamp &ts, const cv::Mat &frame) {
    //         std::unique_lock<std::mutex> lock(cd_frame_mutex);
    //         cd_frame_ts = ts;
    //         frame.copyTo(cd_frame);
    //     });

    // std::string cd_window_name("CD Events");
    // cv::namedWindow(cd_window_name, CV_GUI_NORMAL);
    // cv::resizeWindow(cd_window_name, w, h);
    // cv::moveWindow(cd_window_name, 0, 0);

    // Metavision::Window window("Frames", w, h, Metavision::BaseWindow::RenderMode::BGR);

    Metavision::MTWindow window2("MTWindow BGR", w, h, Metavision::Window::RenderMode::BGR);

    window2.set_keyboard_callback(
        [&window2](Metavision::UIKeyEvent key, int scancode, Metavision::UIAction action, int mods) {
        if (action == Metavision::UIAction::RELEASE && key == Metavision::UIKeyEvent::KEY_ESCAPE) {
                window2.set_close_flag();
        }
    });

    frame_gen.set_output_callback([&](Metavision::timestamp, cv::Mat &frame) {
        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        std::vector<std::vector<cv::Point2f>> markerCorners;
        std::vector<int> markerIds;
        std::vector<cv::Point2f> charucoCorners;
        std::vector<int> charucoIds;

        charuco.detectBoard(gray, charucoCorners, charucoIds, markerCorners, markerIds);

        cv::Mat viz = frame.clone();
        if (!markerIds.empty())
            cv::aruco::drawDetectedMarkers(viz, markerCorners, markerIds);
        if (!charucoIds.empty())
            cv::aruco::drawDetectedCornersCharuco(viz, charucoCorners, charucoIds, cv::Scalar(0, 255, 0));

        window2.show_async(viz);

        // window.show(frame);
    });

    cam.cd().add_callback([&](const Metavision::EventCD *begin, const Metavision::EventCD *end){
        frame_gen.process_events(begin, end);
    });


    cam.start();
    cam.start_recording("test.raw");
    while (cam.is_running() && !window2.should_close()) {
        Metavision::EventLoop::poll_and_dispatch();

    }
    cam.stop_recording("test.raw");
    cam.stop();


    return 0;
}
