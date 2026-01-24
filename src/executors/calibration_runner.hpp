#ifndef YACCP_EXECUTOR_CALIBRATION_RUNNER_HPP
#define YACCP_EXECUTOR_CALIBRATION_RUNNER_HPP
#include "../cli/orchestrator.hpp"

namespace YACCP::Executor {
    void runCalibration(CLI::CliCmdConfig& cliCmdConfig,
                        CLI::CliCmds& cliCmds,
                        std::filesystem::path path,
                        const std::stringstream& dateTime);
} // YACCP::Executor

#endif //YACCP_EXECUTOR_CALIBRATION_RUNNER_HPP