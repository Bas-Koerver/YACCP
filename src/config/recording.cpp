#include "recording.hpp"

#include <stdexcept>

#include "orchestrator.hpp"
#include "../recoding/recorders/camera_worker.hpp"

namespace YACCP::Config {
    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config) {
        // [recording] configuration variables.
        // const toml::node_view recording{tbl["recording"]};
        const auto* recordingTbl{tbl["recording"].as_table()};
        if (!recordingTbl)
            throw std::runtime_error("Missing [recording] table");

        // Global [recording] variables.
        config.fps = recordingTbl->get("fps")->value_or(30);


        const auto* workerArray{(*recordingTbl)["workers"].as_array()};
        if (!workerArray)
            throw std::runtime_error("Missing [[recording.workers]] array");

        config.workers.reserve(workerArray->size());

        for (const toml::node& workerNode : *workerArray) {
            const toml::table* workerTbl = workerNode.as_table();
            if (!workerTbl)
                throw std::runtime_error("Each [[recording.workers]] entry must be a table");

            RecordingConfig::Worker worker{};

            const WorkerTypes type{stringToWorkerType(requireVariable<std::string>(*workerTbl, "type"))};
            worker.placement = requireVariable<int>(*workerTbl, "placement");
            worker.masterWorker = workerTbl->get("masterWorker")->value_or(false);
            worker.camUuid = workerTbl->get("camUuid")->value_or("");

            switch (type) {
            case WorkerTypes::basler: {
                worker.backend = Basler{};
                break;
            }
            case WorkerTypes::prophesee: {
                Prophesee prophesee{};
                prophesee.accumulationTime = workerTbl->get("accumulation_time")->value_or(33333);
                prophesee.saveEventFile = workerTbl->get("save_event_file")->value_or(false);
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

    //
    // if (fpsOpt && !accumulationTimeOpt) {
    //     config.fps = *fpsOpt;
    //     config.prophesee.accumulationTime = static_cast<int>(std::floor(1. / config.fps * 1e6));
    // } else if (!fpsOpt && accumulationTimeOpt) {
    //     config.fps = static_cast<int>(std::floor(1e6 / *accumulationTimeOpt));
    //     config.prophesee.accumulationTime = *accumulationTimeOpt;
    // } else if (fpsOpt && accumulationTimeOpt) {
    //     config.fps = *fpsOpt;
    //     config.prophesee.accumulationTime = *accumulationTimeOpt;
    // } else {
    //     config.fps = 30;
    //     config.prophesee.accumulationTime = static_cast<int>(std::floor(1. / 30 * 1e6));
    // }
    //
    // if (const auto *slaveWorkerArray{recording["slave_workers"].as_array()}) {
    //     numWorkers += slaveWorkerArray->size();
    //     for (std::size_t i{0} ; i < slaveWorkerArray->size() ; ++i) {
    //         const auto element{(*slaveWorkerArray)[i].value<std::string>()};
    //
    //         if (!element)
    //             throw std::runtime_error("slave_worker[" + std::to_string(i) + "] must be a string");
    //
    //         config.slaveWorkers.emplace_back(stringToWorkerType(*element));
    //     }
    // }
    //
    // const auto &masterWorker {recording["master_worker"].as_string()};
    // if (!masterWorker) throw std::runtime_error("Missing master worker");
    //
    // numWorkers += 1;
    // config.masterWorker = stringToWorkerType(masterWorker->get());
    //
    // if (const auto *camPlacementArray{recording["cam_placement"].as_array()}) {
    //     for (std::size_t i{0} ; i < camPlacementArray->size() ; ++i) {
    //         const auto element{(*camPlacementArray)[i].value<int>()};
    //
    //         if (!element)
    //             throw std::runtime_error("cam_placement[" + std::to_string(i) + "] must be an integer");
    //
    //         config.camPlacement.emplace_back(*element);
    //     }
    //
    //     if (!camPlacementArray->size() == numWorkers)
    //         throw std::runtime_error("Too many or too few indexes defined in recording.cam_placement");
    // }
    //
    // const auto masterCameraOpt = recording["master_camera"].value<int>();
    // if (!masterCameraOpt)
    //     throw std::runtime_error("recording.master_camera is required");
    // config.masterCamera = *masterCameraOpt;
    //
    // // [prophesee] worker
    // const toml::node_view prophesee{recording["workers"]["prophesee"]};
    // config.prophesee.saveEventFile = prophesee["save_eventfile"].value_or(false);
    //
    // if (const auto *camUuidArray{prophesee["cam_uuid"].as_array()}) {
    //     for (std::size_t i{0} ; i < camUuidArray->size() ; ++i) {
    //         const auto element{(*camUuidArray)[i].value<std::string>()};
    //
    //         if (!element)
    //             throw std::runtime_error("prophesee.cam_uuid[" + std::to_string(i) + "] must be a string");
    //
    //         config.prophesee.camUuid.emplace_back(*element);
    //     }
    // }
    //
    // // [basler] worker
    // const toml::node_view basler{recording["workers"]["basler"]};
    // if (const auto *camUuidArray{basler["cam_uuid"].as_array()}) {
    //     for (std::size_t i{0} ; i < camUuidArray->size() ; ++i) {
    //         const auto element{(*camUuidArray)[i].value<std::string>()};
    //
    //         if (!element)
    //             throw std::runtime_error("prophesee.cam_uuid[" + std::to_string(i) + "] must be a string");
    //
    //         config.basler.camUuid.emplace_back(*element);
    //     }
    // }
} // YACCP::Config
