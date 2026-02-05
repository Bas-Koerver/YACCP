#ifndef YACCP_SRC_RECORDING_RECORDERS_PROPHESEE_CAM_WORKER_HPP
#define YACCP_SRC_RECORDING_RECORDERS_PROPHESEE_CAM_WORKER_HPP
#include <metavision/sdk/stream/camera.h>

#include "camera_worker.hpp"


namespace YACCP {
    struct CamData;

    class PropheseeCamWorker final : public CameraWorker {
    public:
        PropheseeCamWorker(std::stop_source stopSource,
                           std::vector<CamData>& camDatas,
                           Config::RecordingConfig& recordingConfig,
                           Config::Prophesee& configBackend,
                           int index,
                           const std::filesystem::path& jobPath);

        static void listAvailableSources();

        void start() override;


    private:
        Config::Prophesee& configBackend_;

        void configureBiases(Metavision::Device& device) const;

        void configureEventRateControl(Metavision::Device& device) const;

        void configureEventTrailFilter(Metavision::Device& device) const;

        void configureTimingInterfaces(Metavision::Device& device);

        void configureFacilities(Metavision::Camera& camera);
    };
} // YACCP
#endif //YACCP_SRC_RECORDING_RECORDERS_PROPHESEE_CAM_WORKER_HPP