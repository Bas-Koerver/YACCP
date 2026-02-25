#include "recording.hpp"

#include "orchestrator.hpp"
#include "../global_variables/config_defaults.hpp"

namespace {
    bool compareByIndex(const YACCP::Config::RecordingConfig::Worker& a,
                        const YACCP::Config::RecordingConfig::Worker& b) {
        return a.placement < b.placement;
    }
}


namespace YACCP::Config {
    void parseRecordingConfig(const toml::table& tbl, RecordingConfig& config) {
        // [recording] table.
        const auto* recordingTbl{tbl["recording"].as_table()};
        if (!recordingTbl) {
            throw std::runtime_error("Missing [recording] table");
        }

        // [[recording.workers]] array of tables.
        const auto* workerArray{(*recordingTbl)["workers"].as_array()};
        if (!workerArray) {
            throw std::runtime_error("Missing [[recording.workers]] array");
        }

        std::unordered_map<WorkerTypes, std::size_t> typeCounts;

        // Global [recording] variables.
        config.fps = (*recordingTbl)["fps"].value_or(GlobalVariables::recordingFps);
        config.detectionInterval = (*recordingTbl)["detection_interval"].value_or(GlobalVariables::detectionInterval);
        config.masterWorker = requireVariable<int>(*recordingTbl, "master_worker", "recording");

        // Check whether the defined masterWorker variables is a natural number N
        // and does not exceed the number of workers
        if (config.masterWorker + 1 > workerArray->size() || config.masterWorker < 0) {
            throw std::runtime_error(
                "Invalid 'master_worker' index");
        }

        config.workers.reserve(workerArray->size());

        // Go through the array of tables.
        for (const toml::node& workerNode : *workerArray) {
            const toml::table* workerTbl = workerNode.as_table();
            if (!workerTbl) throw std::runtime_error("Each [[recording.workers]] entry must be a table");
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

            if (worker.placement + 1 > workerArray->size() || worker.placement < 0) {
                throw std::runtime_error("Invalid 'placement' index");
            }
            if (typeCounts[type] > 1) {
                worker.camUuid = requireVariable<std::string>(*workerTbl, "cam_uuid", "[recording.workers]");
            } else {
                worker.camUuid = (*workerTbl)["cam_uuid"].value_or("");
            }

            // Based on the type load in the extra variables that are unique to that specific type.
            switch (type) {
            case WorkerTypes::basler: {
                Basler basler{};

                basler.pixelFormat = workerTbl->contains("pixel_format")
                                         ? std::optional{(*workerTbl)["pixel_format"].value<std::string>()}
                                         : std::nullopt;
                basler.exposureTime = workerTbl->contains("exposure_time")
                                          ? std::optional{(*workerTbl)["exposure_time"].value<double>()}
                                          : std::nullopt;
                basler.exposureAuto = workerTbl->contains("exposure_auto")
                                          ? std::optional{(*workerTbl)["exposure_auto"].value<std::string>()}
                                          : std::nullopt;
                basler.autoExposureLowerLimit = workerTbl->contains("auto_exposure_lower_limit")
                                                    ? std::optional{
                                                        (*workerTbl)["auto_exposure_lower_limit"].value<double>()
                                                    }
                                                    : std::nullopt;
                basler.autoExposureUpperLimit = workerTbl->contains("auto_exposure_upper_limit")
                                                    ? std::optional{
                                                        (*workerTbl)["auto_exposure_upper_limit"].value<double>()
                                                    }
                                                    : std::nullopt;
                basler.gainAuto = workerTbl->contains("gain_auto")
                                      ? std::optional{(*workerTbl)["gain_auto"].value<std::string>()}
                                      : std::nullopt;
                basler.autoGainLowerLimit = workerTbl->contains("auto_gain_lower_limit")
                                                ? std::optional{(*workerTbl)["auto_gain_lower_limit"].value<double>()}
                                                : std::nullopt;
                basler.autoGainUpperLimit = workerTbl->contains("auto_gain_upper_limit")
                                                ? std::optional{(*workerTbl)["auto_gain_upper_limit"].value<double>()}
                                                : std::nullopt;
                basler.autoTargetBrightness = workerTbl->contains("auto_target_brightness")
                                                  ? std::optional{
                                                      (*workerTbl)["auto_target_brightness"].value<double>()
                                                  }
                                                  : std::nullopt;
                basler.autoFunctionProfile = workerTbl->contains("auto_function_profile")
                                                 ? std::optional{
                                                     (*workerTbl)["auto_function_profile"].value<std::string>()
                                                 }
                                                 : std::nullopt;

                const auto* lineIoArray{(*workerTbl)["line_io"].as_array()};

                if (lineIoArray) {
                    basler.lineIos.emplace();
                    basler.lineIos->reserve(lineIoArray->size());

                    for (const toml::node& linIoNode : *lineIoArray) {
                        const toml::table* lineIoTbl = linIoNode.as_table();
                        std::vector<std::string> lineIo;

                        lineIo.emplace_back(
                            requireVariable<std::string>(*lineIoTbl, "line", "[recording.workers.line_io]"));
                        lineIo.emplace_back(
                            requireVariable<std::string>(*lineIoTbl, "mode", "[recording.workers.line_io]"));
                        lineIo.emplace_back(
                            requireVariable<std::string>(*lineIoTbl, "source", "[recording.workers.line_io]"));

                        basler.lineIos->emplace_back(lineIo);
                    }
                } else {
                    basler.lineIos = std::nullopt;
                }
                worker.configBackend = basler;
                break;
            }
            case WorkerTypes::prophesee: {
                Prophesee prophesee{};
                prophesee.accumulationTime = (*workerTbl)["accumulation_time"].value_or(
                    GlobalVariables::accumulationTime);
                prophesee.saveEventFile = (*workerTbl)["save_event_file"].value_or(false);
                prophesee.fallingEdgePolarity = requireVariable<int>(*workerTbl,
                                                                     "falling_edge_polarity",
                                                                     "[recording.workers]");

                // Biases
                prophesee.biasDiff = workerTbl->contains("bias_diff")
                                         ? std::optional{(*workerTbl)["bias_diff"].value<int>()}
                                         : std::nullopt;
                prophesee.biasDiffOn = workerTbl->contains("bias_diff_on")
                                           ? std::optional{(*workerTbl)["bias_diff_on"].value<int>()}
                                           : std::nullopt;
                prophesee.biasDiffOff = workerTbl->contains("bias_diff_off")
                                            ? std::optional{(*workerTbl)["bias_diff_off"].value<int>()}
                                            : std::nullopt;
                prophesee.biasFo = workerTbl->contains("bias_fo")
                                       ? std::optional{(*workerTbl)["bias_fo"].value<int>()}
                                       : std::nullopt;
                prophesee.biasHpf = workerTbl->contains("bias_hpf")
                                        ? std::optional{(*workerTbl)["bias_hpf"].value<int>()}
                                        : std::nullopt;
                prophesee.biasRefr = workerTbl->contains("bias_refr")
                                         ? std::optional{(*workerTbl)["bias_refr"].value<int>()}
                                         : std::nullopt;

                // Event rate controller
                prophesee.ercEnabled = (*workerTbl)["enable_erc"].value_or(GlobalVariables::ercEnabled);
                if (prophesee.ercEnabled) {
                    prophesee.ercRate = workerTbl->contains("erc_rate")
                                            ? std::optional{(*workerTbl)["erc_rate"].value<int>()}
                                            : std::nullopt;
                }

                // Event trail filter
                prophesee.etfEnabled = (*workerTbl)["enable_etf"].value_or(GlobalVariables::etfEnabled);
                if (prophesee.etfEnabled) {
                    prophesee.etfMode = workerTbl->contains("etf_mode")
                                            ? std::optional{
                                                stringToEftMode((*workerTbl)["etf_mode"].value<std::string>().value())
                                            }
                                            : std::nullopt;
                    prophesee.etfThreshold = workerTbl->contains("etf_threshold")
                                                 ? std::optional{(*workerTbl)["etf_threshold"].value<int>()}
                                                 : std::nullopt;
                }

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