#include "recording.hpp"

namespace YACCP::CLI {
    ::CLI::App& addRecordingCmd(::CLI::App& app, RecordingCmdConfig& config) {
        ::CLI::App* subCmd = app.add_subcommand("record", "Record detections");
        // ::CLI::Option_group* group = app.add_option_group("listGroup");

        // TODO: Check with and without const
        (void)subCmd->add_flag("-l, --list",
                               [&config](const bool v) {
                                   config.setShowAvailableJobs(v);
                               },
                               "List available jobs");
        (void)subCmd->add_flag("-c, --show-cams",
                               [&config](const bool v) {
                                   config.setShowAvailableCams(v);
                               },
                               "Show all available cameras");
        (void)subCmd->add_option_function<std::string>("-j, --job-id",
                                                 [&config](const std::string& v) {
                                                     config.setJobId(v);
                                                 },
                                                 "Give a specific job ID to record to")
              ->default_str("Latest job ID");
        (void)subCmd->require_option(-1);

        return *subCmd;
    }
} // namespace YACCP
