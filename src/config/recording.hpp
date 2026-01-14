#ifndef YACCP_CONFIG_RECORDING_HPP
#define YACCP_CONFIG_RECORDING_HPP
#include <variant>
#include <toml++/toml.hpp>
#include <nlohmann/json.hpp>

namespace YACCP {
    enum class WorkerTypes;
}

namespace YACCP::Config {
    struct Basler {
    };

    struct Prophesee {
        int accumulationTime{};
        bool saveEventFile{};
    };

    using Backend = std::variant<Basler, Prophesee>;

    struct RecordingConfig {
        struct Worker {
            int placement{};
            bool masterWorker{};
            std::string camUuid{};

            Backend backend;
        };

        int fps{};
        std::vector<Worker> workers{};
    };

    std::string workerTypeToString(WorkerTypes workerType);

    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config);

    inline void to_json(nlohmann::json& j, const Prophesee& p) {
        j = {
            {"accumulationTime", p.accumulationTime},
            {"saveEventFile", p.saveEventFile},
        };
    }

    inline void from_json(const nlohmann::json& j, Prophesee& p) {
        j.at("accumulationTime").get_to(p.accumulationTime);
        j.at("saveEventFile").get_to(p.saveEventFile);
    }

    inline void to_json(nlohmann::json& j, const RecordingConfig::Worker& w) {
        j = {
            {"placement", w.placement},
            {"masterWorker", w.masterWorker},
            {"camUuid", w.camUuid}
        };

        std::visit([&](const auto& workerBackend) {
            using T = std::decay_t<decltype(workerBackend)>;

            if constexpr (std::is_same_v<T, Basler>) {
                j["type"] = "basler";
            } else if constexpr (std::is_same_v<T, Prophesee>) {
                j["type"] = "prophesee";
                nlohmann::json bj{workerBackend};
                j.update(bj);
            }
        }, w.backend);
    }

    inline void from_json(const nlohmann::json& j, RecordingConfig::Worker& w) {
        j.at("placement").get_to(w.placement);
        j.at("masterWorker").get_to(w.masterWorker);
        j.at("camUuid").get_to(w.camUuid);

        // Discriminator
        const std::string type = j.at("type");

        // Backend
        if (type == "basler") {
            w.backend = Basler{};
        } else if (type == "prophesee") {
            Prophesee p{};
            j.at("accumulationTime").get_to(p.accumulationTime);
            j.at("saveEventFile").get_to(p.saveEventFile);
        } else {
            throw std::runtime_error("Unknown worker type: " + type);
        }
    }

    inline void to_json(nlohmann::json& j, const RecordingConfig& r) {
        j = {
            {"fps", r.fps},
            {"workers", r.workers},
        };
    }

    inline void from_json(const nlohmann::json& j, RecordingConfig& r) {
        j.at("fps").get_to(r.fps);
        r.workers = j.at("workers").get<std::vector<RecordingConfig::Worker>>();
    }
} // YACCP::Config

#endif //YACCP_CONFIG_RECORDING_HPP
