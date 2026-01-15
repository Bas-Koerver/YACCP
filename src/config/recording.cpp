#include "recording.hpp"
#include "orchestrator.hpp"

#include <iostream>
#include <stdexcept>

#include "../recoding/recorders/camera_worker.hpp"

namespace YACCP::Config {
    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config) {
        // [recording] configuration variables.
        // const toml::node_view recording{tbl["recording"]};
        const auto* recordingTbl{tbl["recording"].as_table()};
        if (!recordingTbl)
            throw std::runtime_error("Missing [recording] table");

        // Global [recording] variables.
        config.fps = (*recordingTbl)["fps"].value_or(30);
        config.masterWorker = requireVariable<int>(*recordingTbl, "master_worker");


        const auto* workerArray{(*recordingTbl)["workers"].as_array()};
        if (!workerArray)
            throw std::runtime_error("Missing [[recording.workers]] array");

        if (config.masterWorker > workerArray->size() || config.masterWorker < 0)
            throw std::runtime_error("Invalid master worker index");

        config.workers.reserve(workerArray->size());

        for (const toml::node& workerNode : *workerArray) {
            const toml::table* workerTbl = workerNode.as_table();
            if (!workerTbl)
                throw std::runtime_error("Each [[recording.workers]] entry must be a table");

            RecordingConfig::Worker worker{};

            const WorkerTypes type{stringToWorkerType(requireVariable<std::string>(*workerTbl, "type"))};
            worker.placement = requireVariable<int>(*workerTbl, "placement");

            worker.camUuid = (*workerTbl)["cam_uuid"].value_or("");

            switch (type) {
            case WorkerTypes::basler: {
                worker.backend = Basler{};
                break;
            }
            case WorkerTypes::prophesee: {
                Prophesee prophesee{};
                prophesee.accumulationTime = (*workerTbl)["accumulation_time"].value_or(33333);
                prophesee.saveEventFile = (*workerTbl)["save_event_file"].value_or(false);
                worker.backend = prophesee;
                break;
            }
            default: {
                throw std::runtime_error("Unknown recording type");
                break;
            }
            }

            config.workers.emplace_back(std::move(worker));
        }
    }


} // YACCP::Config
