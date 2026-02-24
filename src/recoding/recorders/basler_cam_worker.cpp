#define NOMINMAX
#include "basler_cam_worker.hpp"

#include "../job_data.hpp"

#include "../../global_variables/config_defaults.hpp"

#include <opencv2/imgproc.hpp>

namespace {
    class FrameHandler : public Pylon::CImageEventHandler {
    public:
        FrameHandler(std::mutex& m, cv::Mat& f, int& index, GenApi::INodeMap& np) :
            frame_mutex_{m},
            frame_{f},
            id_{index},
            nodeMap_{np} {
        }


        void OnImageGrabbed(Pylon::CInstantCamera& cam, const Pylon::CGrabResultPtr& ptrGrabResult) override {
            if (!ptrGrabResult->GrabSucceeded()) {
                return;
            }

            const int width{static_cast<int>(ptrGrabResult->GetWidth())};
            const int height{static_cast<int>(ptrGrabResult->GetHeight())};
            cv::Mat tempFrameBGR;
            int tempIndex{};

            //BayerRG8 (RGGB)
            cv::Mat tempFrame(height, width, CV_8UC1, ptrGrabResult->GetBuffer());
            cv::cvtColor(tempFrame, tempFrameBGR, cv::COLOR_BayerRGGB2BGR);

            tempIndex = Pylon::CIntegerParameter(nodeMap_, "CounterValue").GetValue();

            {
                std::lock_guard<std::mutex> lock{frame_mutex_};
                tempFrameBGR.copyTo(frame_);
                id_ = tempIndex;
            }
        }


    private:
        std::mutex& frame_mutex_;
        cv::Mat& frame_;
        int& id_;
        GenApi::INodeMap& nodeMap_;
    };


    std::pair<int, int> getDims(GenApi::INodeMap& nodeMap) {
        return {
            static_cast<int>(Pylon::CIntegerParameter(nodeMap, "Width").GetValue()),
            static_cast<int>(Pylon::CIntegerParameter(nodeMap, "Height").GetValue())
        };
    }
}

namespace YACCP {
    void BaslerCamWorker::setPixelFormat(GenApi::INodeMap& nodeMap) const {
        // Configure pixel format and exposure time (FPS).
        if (configBackend_.pixelFormat.has_value()) {
            Pylon::CEnumParameter(nodeMap, "PixelFormat").SetValue(configBackend_.pixelFormat.value().data());
        } else {
            configBackend_.pixelFormat = Pylon::CEnumParameter(nodeMap, "PixelFormat").GetValue();
        }
    }


    void BaslerCamWorker::setExposureControl(GenApi::INodeMap& nodeMap) const {
        if (configBackend_.exposureTime.has_value()) {
            Pylon::CFloatParameter(nodeMap, "ExposureTime").SetValue(configBackend_.exposureTime.value());
        } else {
            // TODO: Maybe change this.
            Pylon::CFloatParameter(nodeMap, "ExposureTime").SetValue(GlobalVariables::exposureTime);
            configBackend_.exposureTime = GlobalVariables::exposureTime;
        }

        if (configBackend_.exposureAuto.has_value()) {
            Pylon::CEnumParameter(nodeMap, "ExposureAuto").SetValue(configBackend_.exposureAuto.value().data());
        } else {
            configBackend_.exposureAuto = Pylon::CEnumParameter(nodeMap, "ExposureAuto").GetValue();
        }

        if (configBackend_.autoExposureLowerLimit.has_value()) {
            Pylon::CFloatParameter(nodeMap, "AutoExposureTimeLowerLimit").SetValue(
                configBackend_.autoExposureLowerLimit.value());
        } else {
            configBackend_.autoExposureLowerLimit = Pylon::CFloatParameter(nodeMap, "AutoExposureTimeLowerLimit").
                GetValue();
        }

        if (configBackend_.autoExposureUpperLimit.has_value()) {
            Pylon::CFloatParameter(nodeMap, "AutoExposureTimeUpperLimit").SetValue(
                configBackend_.autoExposureUpperLimit.value());
        } else {
            configBackend_.autoExposureUpperLimit = Pylon::CFloatParameter(nodeMap, "AutoExposureTimeUpperLimit").
                GetValue();
        }
    }


    void BaslerCamWorker::setGainControl(GenApi::INodeMap& nodeMap) const {
        // TODO: add GainSelector
        if (configBackend_.gainAuto.has_value()) {
            Pylon::CEnumParameter(nodeMap, "GainAuto").SetValue(configBackend_.gainAuto.value().data());
        } else {
            configBackend_.gainAuto = Pylon::CEnumParameter(nodeMap, "GainAuto").GetValue();
        }

        if (configBackend_.autoGainLowerLimit.has_value()) {
            Pylon::CFloatParameter(nodeMap, "AutoGainLowerLimit").SetValue(configBackend_.autoGainLowerLimit.value());
        } else {
            configBackend_.autoGainLowerLimit = Pylon::CFloatParameter(nodeMap, "AutoGainLowerLimit").GetValue();
        }

        if (configBackend_.autoGainUpperLimit.has_value()) {
            Pylon::CFloatParameter(nodeMap, "AutoGainUpperLimit").SetValue(configBackend_.autoGainUpperLimit.value());
        } else {
            configBackend_.autoGainUpperLimit = Pylon::CFloatParameter(nodeMap, "AutoGainUpperLimit").GetValue();
        }
    }


    void BaslerCamWorker::setAutoFunctionControl(GenApi::INodeMap& nodeMap) const {
        // This controls how bright the automatic control will aim for.
        if (configBackend_.autoTargetBrightness.has_value()) {
            Pylon::CFloatParameter(nodeMap, "AutoTargetBrightness").SetValue(
                configBackend_.autoTargetBrightness.value());
        } else {
            configBackend_.autoTargetBrightness = Pylon::CFloatParameter(nodeMap, "AutoTargetBrightness").GetValue();
        }

        if (configBackend_.autoFunctionProfile.has_value()) {
            Pylon::CEnumParameter(nodeMap, "AutoFunctionProfile").SetValue(
                configBackend_.autoFunctionProfile.value().data());
        } else {
            configBackend_.autoFunctionProfile = Pylon::CEnumParameter(nodeMap, "AutoFunctionProfile").GetValue();
        }
    }


    void BaslerCamWorker::setDigitalIo(GenApi::INodeMap& nodeMap) const {
        Pylon::CEnumParameter lineSelector(nodeMap, "LineSelector");

        GenApi::StringList_t lines;
        // Get all possible lines
        lineSelector.GetSymbolics(lines);
        std::vector<std::vector<std::string> > lineIos;

        for (const auto& line : lines) {
            lineSelector.SetValue(line);
            Pylon::CEnumParameter lineMode(nodeMap, "LineMode");
            Pylon::CEnumParameter lineSource(nodeMap, "LineSource");

            if (configBackend_.lineIos.has_value()) {
                std::pair containsLine{false, -1};
                for (auto i{0}; i < configBackend_.lineIos.value().size(); ++i) {
                    if (configBackend_.lineIos.value()[i].front() == line.c_str()) {
                        containsLine.first = true;
                        containsLine.second = i;
                    }
                }

                if (containsLine.first) {
                    if (!configBackend_.lineIos.value()[containsLine.second][1].empty() && lineMode.IsWritable()) {
                        lineMode.SetValue(
                            configBackend_.lineIos.value()[containsLine.second][1].data());
                    }
                    if (!configBackend_.lineIos.value()[containsLine.second].back().empty() && lineSource.
                        IsWritable()) {
                        lineSource.SetValue(
                            configBackend_.lineIos.value()[containsLine.second].back().data());
                    }
                } else {
                    std::vector<std::string> lineIo;
                    lineIo.emplace_back(line);
                    lineIo.emplace_back(lineMode.IsReadable() ? lineMode.GetValue().c_str() : "");
                    lineIo.emplace_back(lineSource.IsReadable() ? lineSource.GetValue().c_str() : "");
                    configBackend_.lineIos.value().emplace_back(lineIo);
                }
            } else {
                std::vector<std::string> lineIo;
                lineIo.emplace_back(line);
                lineIo.emplace_back(lineMode.IsReadable() ? lineMode.GetValue().c_str() : "");
                lineIo.emplace_back(lineSource.IsReadable() ? lineSource.GetValue().c_str() : "");
                lineIos.emplace_back(lineIo);
            }
        }
        if (!configBackend_.lineIos.has_value()) {
            // configBackend_.lineIos.emplace();
            configBackend_.lineIos = lineIos;
        }
    }


    void BaslerCamWorker::setCounters(GenApi::INodeMap& nodeMap) {
        // Enable frame count.
        Pylon::CEnumParameter(nodeMap, "CounterSelector").SetValue("Counter1");
        Pylon::CEnumParameter(nodeMap, "CounterEventSource").SetValue("ExposureActive");
        Pylon::CEnumParameter(nodeMap, "CounterEventActivation").SetValue("RisingEdge");
        Pylon::CCommandParameter(nodeMap, "CounterReset").Execute();
    }


    BaslerCamWorker::BaslerCamWorker(std::stop_source stopSource,
                                     std::vector<CamData>& camDatas,
                                     Config::RecordingConfig& recordingConfig,
                                     Config::Basler& configBackend,
                                     const int index,
                                     const std::filesystem::path& jobPath) :
        CameraWorker(stopSource, camDatas, recordingConfig, index, jobPath),
        configBackend_(configBackend) {
        Pylon::PylonInitialize();
        // TODO: Handle scenarios where the camera doesn't support external triggers
    }


    void BaslerCamWorker::listAvailableSources() {
        Pylon::PylonInitialize();
        Pylon::CTlFactory& TlFactory = Pylon::CTlFactory::GetInstance();
        Pylon::DeviceInfoList_t lstDevices;

        try {
            (void)TlFactory.EnumerateDevices(lstDevices);
            if (lstDevices.empty()) {
                std::cerr << "No available Basler cameras found \n";
                Pylon::PylonTerminate();
            }
        }
        catch (const GenICam::GenericException& e) {
            std::cerr << "Pylon error: " << e.GetDescription() << "\n";
            Pylon::PylonTerminate();
        }

        std::cout << "Available Basler cameras:\n";

        for (auto device : lstDevices) {
            const auto type = device.GetDeviceFactory();
            const auto id = device.GetFullName();
            std::cout << "Source type: " << type << "\n";
            std::cout << "- ID: " << id << "\n";
        }
        std::cout << "\n";
    }


    void BaslerCamWorker::start() {
        Pylon::CInstantCamera cam;

        try {
            Pylon::CTlFactory& TlFactory = Pylon::CTlFactory::GetInstance();
            Pylon::DeviceInfoList_t lstDevices;

            (void)TlFactory.EnumerateDevices(lstDevices);
            if (lstDevices.empty()) {
                camData_.runtimeData.exitCode = 1;
                camData_.runtimeData.e = std::make_exception_ptr(std::runtime_error("No Basler cameras found."));
                stopSource_.request_stop();
                return;
            }

            if (recordingConfig_.workers[index_].camUuid.empty()) {
                cam.Attach(TlFactory.CreateDevice(lstDevices[0]));
                camData_.info.camName = cam.GetDeviceInfo().GetModelName();
                std::cout << "Using Basler device: " << camData_.info.camName << "\n";
                camData_.runtimeData.isOpen.store(true);
            } else {
                cam.Attach(TlFactory.CreateDevice(
                    Pylon::CDeviceInfo().SetFullName(recordingConfig_.workers[index_].camUuid.data())));
                camData_.info.camName = cam.GetDeviceInfo().GetModelName();
                std::cout << "Using Basler device: " << camData_.info.camName << "\n";
                camData_.runtimeData.isOpen.store(true);
            }
        }
        catch (GenICam::GenericException& e) {
            camData_.runtimeData.exitCode = 1;
            camData_.runtimeData.e = std::make_exception_ptr(std::runtime_error(e.GetDescription()));
            (void)stopSource_.request_stop();
            return;
        }
        catch (...) {
            camData_.runtimeData.exitCode = 1;
            camData_.runtimeData.e = std::current_exception();
            (void)stopSource_.request_stop();
            return;
        }

        if (camData_.runtimeData.isOpen.load()) {
            cam.Open();
            GenApi::INodeMap& nodeMap = cam.GetNodeMap();
            int width;
            int height;

            try {
                std::tie(width, height) = getSetNodeMapParameters(nodeMap);
            }
            catch (GenICam::GenericException& e) {
                camData_.runtimeData.exitCode = 1;
                camData_.runtimeData.e = std::make_exception_ptr(std::runtime_error(e.GetDescription()));
                (void)stopSource_.request_stop();
                return;
            }
            catch (...) {
                camData_.runtimeData.exitCode = 1;
                camData_.runtimeData.e = std::current_exception();
                (void)stopSource_.request_stop();
                return;
            }

            camData_.info.resolution.width = width;
            camData_.info.resolution.height = height;

            cv::Mat grayFrame;
            cv::Mat overlay{height, width, CV_8UC3, cv::Scalar(0, 0, 0)};

            std::mutex frameMutex;
            cv::Mat frame;
            int frameIndex;

            FrameHandler frameHandler{frameMutex, frame, frameIndex, nodeMap};
            cam.RegisterImageEventHandler(&frameHandler,
                                          Pylon::RegistrationMode_Append,
                                          Pylon::Cleanup_None);

            cam.StartGrabbing(Pylon::GrabStrategy_LatestImageOnly, Pylon::GrabLoop_ProvidedByInstantCamera);

            camData_.runtimeData.isRunning.store(cam.IsGrabbing());
            if (!camData_.runtimeData.datasetMode) {
                if (camData_.info.isMaster) {
                    requestedFrame_ = 1 + recordingConfig_.fps * recordingConfig_.detectionInterval;
                    for (auto& [info, runtimeData] : camDatas_) {
                        if (info.isMaster) {
                            continue;
                        }
                        (void)runtimeData.frameRequestQ.enqueue(requestedFrame_);
                    }

                    while (cam.IsGrabbing() && !stopToken_.stop_requested()) {
                        cv::Mat localFrame;
                        int localFrameIndex;
                        {
                            std::unique_lock<std::mutex> lock{frameMutex};
                            if (frame.empty()) {
                                continue;
                            }
                            localFrame = frame.clone();
                            localFrameIndex = frameIndex;
                        }
                        // Frame used for visualisation
                        {
                            std::unique_lock<std::mutex> lock{camData_.runtimeData.m};
                            camData_.runtimeData.frame = localFrame.clone();
                        }

                        if (localFrameIndex >= requestedFrame_) {
                            VerifyTask frameData;
                            frameData.id = requestedFrame_;
                            frameData.frame = localFrame.clone();
                            (void)camData_.runtimeData.frameVerifyQ.enqueue(frameData);

                            requestedFrame_ = localFrameIndex + (
                                                  recordingConfig_.fps * recordingConfig_.detectionInterval);

                            for (auto& [info, runtimeData] : camDatas_) {
                                if (info.isMaster) {
                                    continue;
                                }
                                (void)runtimeData.frameRequestQ.enqueue(requestedFrame_);
                            }
                        }
                    }
                } else {
                    // TODO: Add slave mode.
                    throw std::logic_error("Function not yet implemented");
                }

                cam.StopGrabbing();
                cam.Close();
                (void)cam.DeregisterImageEventHandler(&frameHandler);
            } else {
                while (cam.IsGrabbing() && !stopToken_.stop_requested()) {
                    cv::Mat localFrame;
                    int localFrameIndex;
                    {
                        std::unique_lock<std::mutex> lock{frameMutex};
                        if (frame.empty()) {
                            continue;
                        }
                        localFrame = frame.clone();
                        localFrameIndex = frameIndex;
                    }
                    // Frame used for visualisation
                    {
                        std::unique_lock<std::mutex> lock{camData_.runtimeData.m};
                        camData_.runtimeData.frame = localFrame.clone();
                    }

                }
            }
        } else {
            stopSource_.request_stop();
        }
    }


    BaslerCamWorker::~BaslerCamWorker() {
        Pylon::PylonTerminate();
    }


    std::pair<int, int> BaslerCamWorker::getSetNodeMapParameters(GenApi::INodeMap& nodeMap) const {
        setPixelFormat(nodeMap);
        setExposureControl(nodeMap);
        setGainControl(nodeMap);
        setAutoFunctionControl(nodeMap);
        setDigitalIo(nodeMap);
        setCounters(nodeMap);

        return getDims(nodeMap);
    }
}
