#include "validation.hpp"

namespace YACCP::CLI {
    ::CLI::App* addValidationCmd(::CLI::App& app, ValidationCmdConfig& config) {
        ::CLI::App* subCmd = app.add_subcommand("validate", "Validate recorded images");

        subCmd->add_flag("-l, --list", config.showAvailableJobs, "List available jobs");
        subCmd->add_option("-j, --job-id", config.jobId, "Give a specific job ID to validate");

        subCmd->require_option(1);

        return subCmd;
    }
} // namespace YACCP::CLI