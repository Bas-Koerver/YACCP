#ifndef STEREO_CAMERA_CALIBRATION_CAMERA_WORKER_HPP
#define STEREO_CAMERA_CALIBRATION_CAMERA_WORKER_HPP
#include <mutex>
#include <readerwriterqueue.h>
#include <opencv2/core/mat.hpp>


namespace YACCP {
    struct VerifyTask;
    /**
     * @brief Simple enum to represent different camera worker types.
     */
    enum class WorkerTypes {
        propheseeDvsEvk4,
        baslerRgb,
    };

    /**
     * @brief All data concerning a camera worker.
     *
     * @param isMaster Indicates if this camera is the master camera.
     * @param isOpen Whenever the camera has been opened successfully.
     * @param isRunning Whenever the camera is running.
     * @param exitCode Exit code of the camera worker thread.
     * @param camName A string representing the camera name for user feedback.
     * @param width The width resolution of the camera.
     * @param height The height resolution of the camera.
     * @param frame The latest frame captured by the camera.
     * @param m Mutex to protect access to the frame.
     * @param frameRequestQ Queue to request a new frame from the slave cameras.
     * @param frameVerifyQ Queue to send frames to the verification thread.
     */
    struct CamData {
        // Thread information
        bool isMaster;
        bool isOpen{false};
        bool isRunning{false};
        int exitCode;
        // Camera information
        std::string camName;
        int width{};
        int height{};
        cv::Mat frame;
        std::mutex m;
        // Communication
        moodycamel::ReaderWriterQueue<int> frameRequestQ{100};
        moodycamel::BlockingReaderWriterQueue<VerifyTask> frameVerifyQ{100};
        // Viewing details
        int x;
        int y;
    };


    /**
     * @brief Parent class for camera workers.
     */
    class CameraWorker {
    public:
        /**
         * @brief Camera worker constructor.
         *
         * @param stopSource Stop source to control the program execution.
         * @param camDatas Reference to the camera data struct.
         * @param fps Frames per second to capture at.
         * @param id ID of the camera worker this must be the index of the correct CamData from camDatas.
         * @param camId A user can give a specific camera ID to connect to.
         */
        CameraWorker(std::stop_source stopSource,
                     std::vector<CamData> &camDatas,
                     int fps,
                     int id,
                     std::string camId = {});

        /**
         * @brief Function to list available camera sources.
         */
        virtual void listAvailableSources();

        /**
         * @brief Function to start the camera worker.
         */
        virtual void start();

        virtual ~CameraWorker() = default;

    protected:
        std::stop_source stopSource_;
        std::stop_token stopToken_;
        std::vector<CamData> &camDatas_;
        CamData &camData_;
        const int fps_;
        const int id_;
        // A user can give a specific camera ID to connect to.
        std::string camId_;
    };
} // YACCP

#endif //STEREO_CAMERA_CALIBRATION_CAMERA_WORKER_HPP
