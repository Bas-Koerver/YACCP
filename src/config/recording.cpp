#include "recording.hpp"

#include "orchestrator.hpp"

#include "../global_variables/config_defaults.hpp"

#include <boost/algorithm/string.hpp>


namespace YACCP::Config {
    WorkerTypes stringToWorkerType(std::string worker) {
        boost::algorithm::to_lower(worker); // make string lowercase
        if (const auto it = workerTypesMap.find(worker); it != workerTypesMap.end()) {
            return it->second;
        }
        throw std::runtime_error("Unknown worker type: " + worker);
    }

    std::string workerTypeToString(const WorkerTypes workerType) {
        for (const auto& [key, value] : workerTypesMap) {
            if (value == workerType) return key;
        }
        return "Not found";
    }

    bool compareByIndex(const RecordingConfig::Worker &a, const RecordingConfig::Worker &b) {
        return a.placement < b.placement;
    }

    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config) {
        // [recording] table.
        const auto* recordingTbl{tbl["recording"].as_table()};
        if (!recordingTbl)
            throw std::runtime_error("Missing [recording] table");

        // [[recording.workers]] array of tables.
        const auto* workerArray{(*recordingTbl)["workers"].as_array()};
        if (!workerArray)
            throw std::runtime_error("Missing [[recording.workers]] array");

        std::unordered_map<WorkerTypes, std::size_t> typeCounts;

        // Global [recording] variables.
        config.fps = (*recordingTbl)["fps"].value_or(GlobalVariables::recordingFps);
        config.masterWorker = requireVariable<int>(*recordingTbl, "master_worker", "recording");

        // Check whether the defined masterWorker variables is a natural number N
        // and does not exceed the number of workers
        if (config.masterWorker + 1 > workerArray->size() || config.masterWorker < 0)
            throw std::runtime_error("Invalid 'master_worker' index");

        config.workers.reserve(workerArray->size());

        // Go through the array of tables.
        for (const toml::node& workerNode : *workerArray) {
            const toml::table* workerTbl = workerNode.as_table();
            if (!workerTbl)
                throw std::runtime_error("Each [[recording.workers]] entry must be a table");
            const WorkerTypes type{
                stringToWorkerType(requireVariable<std::string>(*workerTbl, "type", "[recording.workers]"))
            };
            typeCounts[type]++;
        }

        // Go through the array of tables.
        for (const toml::node& workerNode : *workerArray) {
            const toml::table* workerTbl = workerNode.as_table();
            RecordingConfig::Worker worker{};

            // Global worker variables these are the same regardless of the worker type.
            const WorkerTypes type{
                stringToWorkerType(requireVariable<std::string>(*workerTbl, "type", "[recording.workers]"))
            };
            worker.placement = requireVariable<int>(*workerTbl, "placement", "[recording.workers]");

            if (worker.placement + 1 > workerArray->size() || worker.placement < 0)
                throw std::runtime_error("Invalid 'placement' index"
                    );
            if (typeCounts[type] > 1) {
                worker.camUuid = requireVariable<std::string>(*workerTbl, "cam_uuid", "[recording.workers]");
            } else {
                worker.camUuid = (*workerTbl)["cam_uuid"].value_or("");
            }

            // Based on the type load in the extra variables that are unique to that specific type.
            switch (type) {
            case WorkerTypes::basler: {
                worker.configBackend = Basler{};
                break;
            }
            case WorkerTypes::prophesee: {
                Prophesee prophesee{};
                prophesee.accumulationTime = (*workerTbl)["accumulation_time"].value_or(GlobalVariables::accumulationTime);
                prophesee.saveEventFile = (*workerTbl)["save_event_file"].value_or(false);
                prophesee.fallingEdgePolarity = requireVariable<int>(*workerTbl,
                                                                     "falling_edge_polarity",
                                                                     "[recording.workers]");
                worker.configBackend = prophesee;
                break;
            }
            default: {
                throw std::runtime_error("Unknown recording type");
            }
            }

            config.workers.emplace_back(std::move(worker));
        }
        std::ranges::sort(config.workers, compareByIndex);
    }
} // YACCP::Config
