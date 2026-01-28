#include "validation.hpp"

namespace YACCP::CLI {
    ::CLI::App& addValidationCmd(::CLI::App& app, ValidationCmdConfig& config) {
        ::CLI::App* subCmd = app.add_subcommand("validate", "Validate recorded images");

        (void)subCmd->add_flag("-l, --list", [&config](bool v) { config.setShowAvailableJobs(v);}, "List available jobs");
        (void)subCmd->add_option_function<std::string>("-j, --job-id", [&config](const std::string& v) {config.setJobId(v);}, "Give a specific job ID to validate");

        (void)subCmd->require_option(1);

        return *subCmd;
    }
} // namespace YACCP::CLI