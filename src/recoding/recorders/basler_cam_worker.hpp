#ifndef YACCP_SRC_RECORDING_RECORDERS_BASLER_CAM_WORKER_HPP
#define YACCP_SRC_RECORDING_RECORDERS_BASLER_CAM_WORKER_HPP
#include "camera_worker.hpp"

#include <pylon/PylonIncludes.h>


namespace YACCP {
    class BaslerCamWorker final : public CameraWorker {
    public:
        /**
        * @copydoc YACCP::CameraWorker::CameraWorker
        */
        BaslerCamWorker(std::stop_source stopSource,
                        std::vector<CamData>& camDatas,
                        Config::RecordingConfig& recordingConfig,
                        Config::Basler& configBackend,
                        int index,
                        const std::filesystem::path& jobPath);

        static void listAvailableSources();

        void start() override;

        ~BaslerCamWorker() override;


    private:
        Config::Basler& configBackend_;
        int requestedFrame_{1}; // Start from frame 1

        void setPixelFormat(GenApi::INodeMap& nodeMap) const;

        void setExposureControl(GenApi::INodeMap& nodeMap) const;

        void setGainControl(GenApi::INodeMap& nodeMap) const;

        void setAutoFunctionControl(GenApi::INodeMap& nodeMap) const;

        void setDigitalIo(GenApi::INodeMap& nodeMap) const;

        static void setCounters(GenApi::INodeMap& nodeMap);

        [[nodiscard]] std::pair<int, int> getSetNodeMapParameters(GenApi::INodeMap& nodeMap) const;
    };
} // YACCP

#endif //YACCP_SRC_RECORDING_RECORDERS_BASLER_CAM_WORKER_HPP