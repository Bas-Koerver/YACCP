#include "prophesee_cam_worker.hpp"

#include "../job_data.hpp"

#include <metavision/hal/facilities/i_erc_module.h>
#include <metavision/hal/facilities/i_event_trail_filter_module.h>
#include <metavision/hal/facilities/i_hw_identification.h>
#include <metavision/hal/facilities/i_ll_biases.h>
#include <metavision/hal/facilities/i_trigger_in.h>
#include <metavision/sdk/core/algorithms/on_demand_frame_generation_algorithm.h>
#include <metavision/sdk/core/algorithms/polarity_filter_algorithm.h>
#include <metavision/sdk/core/utils/cd_frame_generator.h>

namespace YACCP {
    std::string sourceTypeToString(Metavision::OnlineSourceType type) {
        switch (type) {
        case Metavision::OnlineSourceType::EMBEDDED:
            return "EMBEDDED";
        case Metavision::OnlineSourceType::USB:
            return "USB";
        case Metavision::OnlineSourceType::REMOTE:
            return "REMOTE";
        default:
            return "UNKNOWN";
        }
    }


    void PropheseeCamWorker::configureBiases(Metavision::Device& device) const {
        const auto biases{device.get_facility<::Metavision::I_LL_Biases>()};
        // bias_diff
        if (configBackend_.biasDiff) {
            if (!biases->set("bias_diff", *configBackend_.biasDiff)) {
                throw std::runtime_error("Failed to set bias_diff, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
        } else {
            configBackend_.biasDiff = biases->get("bias_diff");
        }

        // bias_diff_on
        if (configBackend_.biasDiffOn) {
            if (!biases->set("bias_diff_on", *configBackend_.biasDiffOn)) {
                throw std::runtime_error("Failed to set bias_diff_on, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
        } else {
            configBackend_.biasDiffOn = biases->get("bias_diff_on");
        }

        // bias_diff_off
        if (configBackend_.biasDiffOff) {
            if (!biases->set("bias_diff_off", *configBackend_.biasDiffOff)) {
                throw std::runtime_error("Failed to set bias_diff_off, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
        } else {
            configBackend_.biasDiffOff = biases->get("bias_diff_off");
        }

        // bias_fo
        if (configBackend_.biasFo) {
            if (!biases->set("bias_fo", *configBackend_.biasFo)) {
                throw std::runtime_error("Failed to set bias_fo, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
        } else {
            configBackend_.biasFo = biases->get("bias_fo");
        }

        // bias_hpf
        if (configBackend_.biasHpf) {
            if (!biases->set("bias_hpf", *configBackend_.biasHpf)) {
                throw std::runtime_error("Failed to set bias_hpf, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
        } else {
            configBackend_.biasHpf = biases->get("bias_hpf");
        }

        // bias_refr
        if (configBackend_.biasRefr) {
            if (!biases->set("bias_refr", *configBackend_.biasRefr)) {
                throw std::runtime_error("Failed to set bias_refr, "
                    "for the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
        } else {
            configBackend_.biasRefr = biases->get("bias_refr");
        }
    }


    void PropheseeCamWorker::configureEventRateControl(Metavision::Device& device) const {
        const auto erc{device.get_facility<Metavision::I_ErcModule>()};
        if (configBackend_.ercEnabled) {
            if (!erc->enable(true)) {
                throw std::runtime_error(
                    "Failed to set event rate controller, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }
            if (configBackend_.ercRate) {
                (void)erc->set_cd_event_rate(static_cast<uint32_t>(*configBackend_.ercRate));
            } else {
                configBackend_.ercRate = erc->get_cd_event_rate();
            }
        }
    }


    void PropheseeCamWorker::configureEventTrailFilter(Metavision::Device& device) const {
        const auto etf{device.get_facility<Metavision::I_EventTrailFilterModule>()};
        if (configBackend_.etfEnabled) {
            if (!etf->enable(true)) {
                throw std::runtime_error("Failed to set event trail filter, "
                    "the camera: " + camData_.info.camName +
                    ". With index: " + std::to_string(camData_.info.camIndexId) +
                    ". Probably doesn't support this.");
            }

            if (configBackend_.etfMode) {
                (void)etf->set_type(*configBackend_.etfMode);
            } else {
                configBackend_.etfMode = etf->get_type();
            }

            if (configBackend_.etfThreshold) {
                (void)etf->set_threshold(static_cast<uint32_t>(*configBackend_.etfThreshold));
            } else {
                configBackend_.etfThreshold = etf->get_threshold();
            }
        }
    }


    void PropheseeCamWorker::configureTimingInterfaces(Metavision::Device& device) {
        const auto i_trigger_in{device.get_facility<Metavision::I_TriggerIn>()};
        (void)i_trigger_in->enable(Metavision::I_TriggerIn::Channel::Main);
    }


    void PropheseeCamWorker::configureFacilities(Metavision::Camera& camera) {
        Metavision::Device& device = camera.get_device();

        configureBiases(device);
        configureEventRateControl(device);
        configureEventTrailFilter(device);

        // TODO: Handle scenarios where the camera doesn't support external triggers
        configureTimingInterfaces(device);
    }


    void printCurrentDevice(Metavision::Camera& cam, CamData& camData) {
        auto& device = cam.get_device();

        // Facility that gives hardware info
        if (auto* hw_id = device.get_facility<Metavision::I_HW_Identification>()) {
            for (const auto& kv : hw_id->get_system_info()) {
                if (kv.first == "device0 name") {
                    std::cout << "Using Prophesee device: " << kv.second << '\n';
                    camData.info.camName = kv.second;
                }
            }
        } else {
            std::cout << "Using Prophesee device (no HW identification facility)\n";
        }
    }


    PropheseeCamWorker::PropheseeCamWorker(std::stop_source stopSource,
                                           std::vector<CamData>& camDatas,
                                           Config::RecordingConfig& recordingConfig,
                                           Config::Prophesee& configBackend,
                                           int index,
                                           const std::filesystem::path& jobPath) :
        CameraWorker(stopSource, camDatas, recordingConfig, index, jobPath),
        configBackend_(configBackend) {
    }


    void PropheseeCamWorker::listAvailableSources() {
        Metavision::AvailableSourcesList sources{Metavision::Camera::list_online_sources()};

        if (sources.empty()) {
            std::cerr << "No available Prophesee cameras found \n";
            return;
        }

        std::cout << "Available Prophesee cameras:\n";
        for (const auto& source : sources) {
            const auto& type = source.first;
            const auto& ids = source.second;
            std::string typeName;

            typeName = sourceTypeToString(type);

            std::cout << "Source type: " << typeName << "\n";

            // Print connected Prophesee cameras
            for (const auto& id : ids) {
                std::cout << "- ID: " << id << "\n";
            }
        }
        std::cout << "\n";
    }


    void PropheseeCamWorker::start() {
        Metavision::Camera cam;

        if (recordingConfig_.workers[index_].camUuid.empty()) {
            try {
                // open the first available camera
                cam = Metavision::Camera::from_first_available();
                camData_.runtimeData.isOpen.store(true);
            }
            catch (...) {
                camData_.runtimeData.e = std::current_exception();
                (void)stopSource_.request_stop();
                return;
            }
        } else {
            try {
                // open the first available camera
                cam = Metavision::Camera::from_serial(recordingConfig_.workers[index_].camUuid);
                camData_.runtimeData.isOpen.store(true);
            }
            catch (...) {
                camData_.runtimeData.e = std::current_exception();
                (void)stopSource_.request_stop();
                return;
            }
        }

        printCurrentDevice(cam, camData_);

        if (camData_.runtimeData.isOpen.load()) {
            // TODO: add runtime error handling
            cv::Mat cdFrame;
            std::mutex cd_frame_mutex;
            Metavision::timestamp cd_frame_ts{0};
            // Set polarity filter to only include events with a positive polarity.
            Metavision::PolarityFilterAlgorithm pol_filter{1};
            const auto& geometry = cam.geometry();
            camData_.info.resolution.width = geometry.get_width();
            camData_.info.resolution.height = geometry.get_height();
            auto requestNew{true};
            auto requestedFrame{0};
            auto masterCamFrameIndex{0};

            // Configure facilities like biases en timing interfaces.
            try {
                configureFacilities(cam);
            }
            catch (...) {
                camData_.runtimeData.e = std::current_exception();
                (void)stopSource_.request_stop();
                return;
            }

            Metavision::CDFrameGenerator cdFrameGenerator{geometry.get_width(), geometry.get_height()};
            cdFrameGenerator.set_display_accumulation_time_us(configBackend_.accumulationTime);

            Metavision::OnDemandFrameGenerationAlgorithm onDemandFrameGenerator{
                geometry.get_width(),
                geometry.get_height(),
                static_cast<uint32_t>(std::round(1e6 / static_cast<double>(recordingConfig_.fps)))
            };

            (void)cdFrameGenerator.start(
                static_cast<std::uint16_t>(recordingConfig_.fps),
                [&cd_frame_mutex, &cdFrame, &cd_frame_ts](const Metavision::timestamp& ts, const cv::Mat& frame) {
                    std::unique_lock<std::mutex> lock(cd_frame_mutex);
                    cd_frame_ts = ts;
                    frame.copyTo(cdFrame);
                });

            (void)cam.ext_trigger().add_callback(
                [this, &onDemandFrameGenerator, &requestNew, &requestedFrame, &masterCamFrameIndex](
                const Metavision::EventExtTrigger* begin,
                const Metavision::EventExtTrigger* end) {
                    if (requestNew) {
                        (void)camData_.runtimeData.frameRequestQ.try_dequeue(requestedFrame);
                        requestNew = false;
                    }

                    for (auto ev = begin; ev != end; ++ev) {
                        // Check for falling edge trigger and increment frame index.
                        if (ev->p == configBackend_.fallingEdgePolarity) {
                            // Handle falling edge trigger event
                            ++masterCamFrameIndex;
                        }
                        // TODO: Add warning when camera is not keeping up.

                        // If a frame was requested and the current frame index matches or
                        // is larger than the requested frame, generate and enqueue it.
                        if (masterCamFrameIndex >= requestedFrame && requestedFrame > 0) {
                            VerifyTask task;
                            task.id = requestedFrame;
                            onDemandFrameGenerator.generate(ev->t, task.frame);
                            (void)camData_.runtimeData.frameVerifyQ.enqueue(task);

                            requestNew = true;
                        }
                    }
                });

            (void)cam.cd().add_callback(
                [&pol_filter, &cdFrameGenerator, &onDemandFrameGenerator](
                const Metavision::EventCD* begin,
                const Metavision::EventCD* end) {
                    std::vector<Metavision::EventCD> polFilterOut;
                    pol_filter.process_events(begin, end, std::back_inserter(polFilterOut));
                    begin = polFilterOut.data();
                    end = begin + polFilterOut.size();

                    cdFrameGenerator.add_events(begin, end);
                    onDemandFrameGenerator.process_events(begin, end);
                });

            // TODO: Make event file recording optional.
            (void)cam.start();
            (void)cam.start_recording(jobPath_ / "event_file.raw");

            // TODO: Add master mode.
            camData_.runtimeData.isRunning.store(cam.is_running());
            while (cam.is_running() && !stopToken_.stop_requested()) {
                cv::Mat localFrame;
                {
                    std::unique_lock<std::mutex> lock(cd_frame_mutex);
                    if (cdFrame.empty()) {
                        std::this_thread::sleep_for(std::chrono::microseconds(100));
                        continue;
                    }
                    localFrame = cdFrame.clone();
                }
                // Frame used for visualisation
                {
                    std::unique_lock<std::mutex> lock{camData_.runtimeData.m};
                    camData_.runtimeData.frame = localFrame.clone();
                }
            }

            (void)cam.stop_recording();
            (void)cam.stop();
        } else {
            stopSource_.request_stop();
        }
    }
}
