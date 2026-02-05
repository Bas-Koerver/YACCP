#ifndef YACCP_SRC_CONFIG_RECORDING_HPP
#define YACCP_SRC_CONFIG_RECORDING_HPP
#include <variant>
#include <metavision/hal/facilities/i_event_trail_filter_module.h>

#include <nlohmann/json.hpp>

#include <toml++/toml.hpp>

namespace YACCP::Config {

    /**
    * @brief Simple enum to represent different camera worker types.
    */
    enum class WorkerTypes {
        prophesee,
        basler,
    };

    Metavision::I_EventTrailFilterModule::Type stringToEftMode(std::string mode);

    std::string etfModeToString(Metavision::I_EventTrailFilterModule::Type eftMode);


    inline std::unordered_map<std::string, WorkerTypes> workerTypesMap{
        {"prophesee", WorkerTypes::prophesee},
        {"basler", WorkerTypes::basler}
    };

    inline std::unordered_map<std::string, Metavision::I_EventTrailFilterModule::Type> eftModesMap{
        {"stc_cut_trail", Metavision::I_EventTrailFilterModule::Type::STC_CUT_TRAIL},
        {"stc_keep_trail", Metavision::I_EventTrailFilterModule::Type::STC_KEEP_TRAIL},
        {"trail", Metavision::I_EventTrailFilterModule::Type::TRAIL}
    };

    struct Basler {
    };

    struct Prophesee {
        int accumulationTime{};
        bool saveEventFile{};
        int fallingEdgePolarity{};

        // https://docs.prophesee.ai/stable/hw/manuals/biases.html
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
        std::optional<Metavision::I_EventTrailFilterModule::Type> etfMode{};
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
        int detectionInterval{};
        int masterWorker{};
        std::vector<Worker> workers{};
    };

    // Logic for loading config structs to JSON and loading it from a JSON back to the appropriate structs.
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
            j["etfMode"] = etfModeToString(*p.etfMode);
            j["etfThreshold"] = p.etfThreshold;
        }
    }


    inline void from_json(const nlohmann::json& j, Prophesee& p) {
        (void)j.at("accumulationTime").get_to(p.accumulationTime);
        (void)j.at("fallingEdgePolarity").get_to(p.fallingEdgePolarity);
        (void)j.at("saveEventFile").get_to(p.saveEventFile);

        (void)j.at("biasDiff").get_to(p.biasDiff);
        (void)j.at("biasDiffOn").get_to(p.biasDiffOn);
        (void)j.at("biasDiffOff").get_to(p.biasDiffOff);
        (void)j.at("biasFo").get_to(p.biasFo);
        (void)j.at("biasHpf").get_to(p.biasHpf);
        (void)j.at("biasRefr").get_to(p.biasRefr);

        (void)j.at("ercEnabled").get_to(p.ercEnabled);
        if (p.ercEnabled) {
            (void)j.at("ercRate").get_to(p.ercRate);
        }

        (void)j.at("etfEnabled").get_to(p.etfEnabled);
        if (p.etfEnabled) {
            p.etfMode = stringToEftMode(j.at("etfMode").get<std::string>());
            (void)j.at("etfThreshold").get_to(p.etfThreshold);
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

        const std::string type = j.at("type");

        // Backend
        if (type == "basler") {
            w.configBackend = Basler{};
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


    WorkerTypes stringToWorkerType(std::string worker);

    std::string workerTypeToString(WorkerTypes workerType);

    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config);
} // YACCP::Config

#endif //YACCP_SRC_CONFIG_RECORDING_HPP
