#ifndef YACCP_SRC_CONFIG_RECORDING_HPP
#define YACCP_SRC_CONFIG_RECORDING_HPP
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include <boost/algorithm/string/case_conv.hpp>

#if YACCP_HAS_METAVISION
#include <metavision/hal/facilities/i_event_trail_filter_module.h>
#endif

#include <nlohmann/json.hpp>

#include <toml++/toml.hpp>

namespace YACCP::Config {
#if YACCP_HAS_METAVISION
    using EventTrailFilterType = Metavision::I_EventTrailFilterModule::Type;
#else
    enum class EventTrailFilterType {
        STC_CUT_TRAIL,
        STC_KEEP_TRAIL,
        TRAIL
    };
#endif

    /**
    * @brief Simple enum to represent different camera worker types.
    */
    enum class WorkerTypes {
        prophesee,
        basler,
    };


    inline std::unordered_map<std::string, WorkerTypes> workerTypesMap{
        {"prophesee", WorkerTypes::prophesee},
        {"basler", WorkerTypes::basler}
    };

    inline std::unordered_map<std::string, EventTrailFilterType> eftModesMap{
        {"stc_cut_trail", EventTrailFilterType::STC_CUT_TRAIL},
        {"stc_keep_trail", EventTrailFilterType::STC_KEEP_TRAIL},
        {"trail", EventTrailFilterType::TRAIL}
    };


    inline WorkerTypes stringToWorkerType(std::string worker) {
        boost::algorithm::to_lower(worker); // make string lowercase
        if (const auto it{workerTypesMap.find(worker)}; it != workerTypesMap.end()) {
            return it->second;
        }
        throw std::runtime_error("Unknown worker type: " + worker);
    }


    inline std::string workerTypeToString(const WorkerTypes workerType) {
        for (const auto& [key, value] : workerTypesMap) {
            if (value == workerType) {
                return key;
            }
        }
        return "Not found";
    }


    inline EventTrailFilterType stringToEftMode(std::string mode) {
        boost::algorithm::to_lower(mode);
        if (const auto it{eftModesMap.find(mode)}; it != eftModesMap.end()) {
            return it->second;
        }
        throw std::runtime_error("Unknown event trail filter mode");
    }


    inline std::string etfModeToString(const EventTrailFilterType eftMode) {
        for (const auto& [key, value] : eftModesMap) {
            if (value == eftMode) {
                return key;
            }
        }
        return "Not found";
    }


    struct Basler {
        // Theses are optional because the defaults will be defined by the camera, not by the software.
        std::optional<std::string> pixelFormat{};
        std::optional<double> exposureTime{};
        std::optional<std::string> exposureAuto{};
        std::optional<double> autoExposureLowerLimit{};
        std::optional<double> autoExposureUpperLimit{};
        std::optional<std::string> gainAuto{};
        std::optional<double> autoGainLowerLimit{};
        std::optional<double> autoGainUpperLimit{};
        std::optional<double> autoTargetBrightness{};
        std::optional<std::string> autoFunctionProfile{};
        std::optional<std::vector<std::vector<std::string> > > lineIos{};
    };

    struct Prophesee {
        int accumulationTime{};
        bool saveEventFile{};
        int fallingEdgePolarity{};

        // https://docs.prophesee.ai/stable/hw/manuals/biases.html
        // Theses are optional because the defaults will be defined by the camera, not by the software.
        std::optional<int> biasDiff{};
        std::optional<int> biasDiffOn{};
        std::optional<int> biasDiffOff{};
        std::optional<int> biasFo{};
        std::optional<int> biasHpf{};
        std::optional<int> biasRefr{};

        // https://docs.prophesee.ai/stable/hw/manuals/esp.html#event-rate-controller-erc
        bool ercEnabled{};
        std::optional<int> ercRate{};

        // https://docs.prophesee.ai/stable/hw/manuals/esp.html#event-trail-filter-stc-trail
        bool etfEnabled{};
        std::optional<EventTrailFilterType> etfMode{};
        std::optional<int> etfThreshold{};
    };

    using ConfigBackend = std::variant<Basler, Prophesee>;

    struct RecordingConfig {
        struct Worker {
            int placement{};
            std::string camUuid{};

            ConfigBackend configBackend;
        };

        int fps{};
        int recordingFpsLimit{};
        int detectionInterval{};
        int masterWorker{};
        std::vector<Worker> workers{};
    };

    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config);

    // Logic for loading config structs to JSON and loading it from a JSON back to the appropriate structs.
    inline void to_json(nlohmann::json& j, const Basler& b) {
        j = {
            {"pixelFormat", b.pixelFormat},
            {"exposureTime", b.exposureTime},
            {"exposureAuto", b.exposureAuto},
            {"autoExposureLowerLimit", b.autoExposureLowerLimit},
            {"autoExposureUpperLimit", b.autoExposureUpperLimit},
            {"gainAuto", b.gainAuto},
            {"autoGainLowerLimit", b.autoGainLowerLimit},
            {"autoGainUpperLimit", b.autoGainUpperLimit},
            {"autoTargetBrightness", b.autoTargetBrightness},
            {"autoFunctionProfile", b.autoFunctionProfile},
        };
        if (b.lineIos) {
            j["lineIos"] = b.lineIos.value();
        }
    }


    inline void from_json(const nlohmann::json& j, Basler& b) {
        if (j.contains("pixelFormat")) {
            (void)j.at("pixelFormat").get_to(b.pixelFormat);
        }
        if (j.contains("exposureTime")) {
            (void)j.at("exposureTime").get_to(b.exposureTime);
        }
        if (j.contains("exposureAuto")) {
            (void)j.at("exposureAuto").get_to(b.exposureAuto);
        }
        if (j.contains("autoExposureLowerLimit")) {
            (void)j.at("autoExposureLowerLimit").get_to(b.autoExposureLowerLimit);
        }
        if (j.contains("autoExposureUpperLimit")) {
            (void)j.at("autoExposureUpperLimit").get_to(b.autoExposureUpperLimit);
        }
        if (j.contains("gainAuto")) {
            (void)j.at("gainAuto").get_to(b.gainAuto);
        }
        if (j.contains("autoGainLowerLimit")) {
            (void)j.at("autoGainLowerLimit").get_to(b.autoGainLowerLimit);
        }
        if (j.contains("autoGainUpperLimit")) {
            (void)j.at("autoGainUpperLimit").get_to(b.autoGainUpperLimit);
        }
        if (j.contains("autoTargetBrightness")) {
            (void)j.at("autoTargetBrightness").get_to(b.autoTargetBrightness);
        }
        if (j.contains("autoFunctionProfile")) {
            (void)j.at("autoFunctionProfile").get_to(b.autoFunctionProfile);
        }
        if (j.contains("lineIos")) {
            (void)j.at("lineIos").get_to(b.lineIos);
        }
    }


    inline void to_json(nlohmann::json& j, const Prophesee& p) {
        j = {
            {"accumulationTime", p.accumulationTime},
            {"fallingEdgePolarity", p.fallingEdgePolarity},
            {"saveEventFile", p.saveEventFile},

            {"biasDiff", p.biasDiff},
            {"biasDiffOn", p.biasDiffOn},
            {"biasDiffOff", p.biasDiffOff},
            {"biasFo", p.biasFo},
            {"biasHpf", p.biasHpf},
            {"biasRefr", p.biasRefr},
        };

        j["ercEnabled"] = p.ercEnabled;
        if (p.ercEnabled) {
            j["ercRate"] = p.ercRate;
        }

        j["etfEnabled"] = p.etfEnabled;
        if (p.etfEnabled) {
            if (p.etfMode.has_value()) {
                j["etfMode"] = etfModeToString(*p.etfMode);
            }
            if (p.etfThreshold.has_value()) {
                j["etfThreshold"] = p.etfThreshold;
            }
        }
    }


    inline void from_json(const nlohmann::json& j, Prophesee& p) {
        (void)j.at("accumulationTime").get_to(p.accumulationTime);
        (void)j.at("fallingEdgePolarity").get_to(p.fallingEdgePolarity);
        (void)j.at("saveEventFile").get_to(p.saveEventFile);

        if (j.contains("biasDiff")) {
            (void)j.at("biasDiff").get_to(p.biasDiff);
        }
        if (j.contains("biasDiffOn")) {
            (void)j.at("biasDiffOn").get_to(p.biasDiffOn);
        }
        if (j.contains("biasDiffOff")) {
            (void)j.at("biasDiffOff").get_to(p.biasDiffOff);
        }
        if (j.contains("biasFo")) {
            (void)j.at("biasFo").get_to(p.biasFo);
        }
        if (j.contains("biasHpf")) {
            (void)j.at("biasHpf").get_to(p.biasHpf);
        }
        if (j.contains("biasRefr")) {
            (void)j.at("biasRefr").get_to(p.biasRefr);
        }

        (void)j.at("ercEnabled").get_to(p.ercEnabled);
        if (p.ercEnabled) {
            (void)j.at("ercRate").get_to(p.ercRate);
        }

        (void)j.at("etfEnabled").get_to(p.etfEnabled);
        if (p.etfEnabled) {
            if (j.contains("etfMode")) {
                p.etfMode = stringToEftMode(j.at("etfMode").get<std::string>());
            }
            if (j.contains("etfThreshold")) {
                (void)j.at("etfThreshold").get_to(p.etfThreshold);
            }
        }
    }


    inline void to_json(nlohmann::json& j, const RecordingConfig::Worker& w) {
        j = {
            {"placement", w.placement},
            {"camUuid", w.camUuid}
        };

        std::visit([&](const auto& workerBackend) {
                       using T = std::decay_t<decltype(workerBackend)>;

                       if constexpr (std::is_same_v<T, Basler>) {
                           j["type"] = "basler";
                           nlohmann::json bj(workerBackend);
                           j.update(bj);
                       } else if constexpr (
                           std::is_same_v<T, Prophesee>) {
                           j["type"] = "prophesee";
                           nlohmann::json bj(workerBackend);
                           j.update(bj);
                       }
                   },
                   w.configBackend);
    }


    inline void from_json(const nlohmann::json& j, RecordingConfig::Worker& w) {
        (void)j.at("placement").get_to(w.placement);
        (void)j.at("camUuid").get_to(w.camUuid);

        // Backend
        if (const std::string type{j.at("type").get<std::string>()}; type == "basler") {
            w.configBackend = j.get<Basler>();
        } else if (type == "prophesee") {
            w.configBackend = j.get<Prophesee>();
        } else {
            throw std::runtime_error("Unknown worker type: " + type);
        }
    }


    inline void to_json(nlohmann::json& j, const RecordingConfig& r) {
        j = {
            {"fps", r.fps},
            // DetectionInterval is a user variable and not needed to recreate an experiment.
            {"masterWorker", r.masterWorker},
            {"workers", r.workers},
        };
    }


    inline void from_json(const nlohmann::json& j, RecordingConfig& r) {
        (void)j.at("fps").get_to(r.fps);
        // DetectionInterval is a user variable and not needed to recreate an experiment.
        (void)j.at("masterWorker").get_to(r.masterWorker);
        r.workers = j.at("workers").get<std::vector<RecordingConfig::Worker> >();
    }
} // YACCP::Config

#endif //YACCP_SRC_CONFIG_RECORDING_HPP