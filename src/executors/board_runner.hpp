#ifndef YACCP_EXECUTOR_BOARD_RUNNER_HPP
#define YACCP_EXECUTOR_BOARD_RUNNER_HPP
#include "../cli/orchestrator.hpp"

namespace YACCP::Executor {
    void runBoardCreation(CLI::CliCmdConfig& cliCmdConfig, std::filesystem::path path, const std::stringstream& dateTime);
} // YACCP::Executor

#endif //YACCP_EXECUTOR_BOARD_RUNNER_HPP
