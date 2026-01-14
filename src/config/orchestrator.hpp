#ifndef YACCP_CONFIG_ORCHESTRATOR_HPP
#define YACCP_CONFIG_ORCHESTRATOR_HPP
#include <filesystem>

#include "board.hpp"
#include "detection.hpp"
#include "recording.hpp"
#include "viewing.hpp"

namespace YACCP::Config {
    struct FileConfig {
        BoardConfig boardConfig;
        DetectionConfig detectionConfig;
        RecordingConfig recordingConfig;
        ViewingConfig viewingConfig;
    };

    template <class T>
    [[nodiscard]] T requireVariable(const toml::table &tbl, std::string_view key) {
        if (auto value = tbl[key].value<T>())
            return *value;
        throw std::runtime_error("Variable: " + std::string(key) + " missing or invalid");
    }

    WorkerTypes stringToWorkerType(const std::string& worker);

    std::string workerTypeToString(WorkerTypes workerType);

    void loadConfig(FileConfig& config, const std::filesystem::path& path);

    inline void to_json(nlohmann::json& j, const FileConfig& f) {
        j = {
            {"boardConfig", f.boardConfig},
            {"detectionConfig", f.detectionConfig},
            {"recordingConfig", f.recordingConfig}
        };
    }

    inline void from_json(const nlohmann::json& j, FileConfig& f) {
        j.at("boardConfig").get_to(f.boardConfig);
        j.at("detectionConfig").get_to(f.detectionConfig);
        j.at("recordingConfig").get_to(f.recordingConfig);
    }
} // YACCP::Config

#endif //YACCP_CONFIG_ORCHESTRATOR_HPP