#include "recording.hpp"

namespace YACCP::CLI {
    RecordingCmds addRecordingCmd(::CLI::App& app, RecordingCmdConfig& config) {
        RecordingCmds recordingCmds;

        recordingCmds.record = app.add_subcommand("record", "Record detections");

        recordingCmds.record->add_flag("-l, --list", config.showAvailableJobs, "List available jobs");
        recordingCmds.record->add_option("-j, --job-id", config.jobId, "Give a specific job ID to record to")->
                      default_str(
                          "Latest job ID");
        recordingCmds.record->add_flag("-c, --show-cams", config.showAvailableCams, "Show all available cameras");

        recordingCmds.record->parse_complete_callback([&config] {
            if (config.showAvailableJobs && config.showAvailableCams) {
                throw std::runtime_error(
                    "Show available jobs and show available cameras are mutually exclusive, you can only use one at a time\n");
            }
        });

        recordingCmds.calib_images = recordingCmds.record->add_subcommand(
            "calib-images",
            "Record detections of calibration patterns");
        recordingCmds.dataset = recordingCmds.record->add_subcommand("dataset", "Record for creating a dataset");

        return recordingCmds;
    }
} // namespace YACCP