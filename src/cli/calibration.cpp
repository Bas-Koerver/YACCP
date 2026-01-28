#include "calibration.hpp"

namespace YACCP::CLI {
    CalibrationCmds::CalibrationCmds(::CLI::App& calibration, ::CLI::App& mono, ::CLI::App& stereo) noexcept :
        calibration_(calibration),
        mono_(mono),
        stereo_(stereo) {
    };


    CalibrationCmds addCalibrationCmds(::CLI::App& app, CalibrationCmdConfig& config) {
        ::CLI::App* const calibration = app.add_subcommand("calibrate", "Global calibration command");
        ::CLI::App* mono = calibration->add_subcommand("mono", "Mono calibration");
        ::CLI::App* stereo = calibration->add_subcommand("stereo", "stereo calibration");

        (void)calibration->add_flag("-l, --list",
                                    [&config](const bool v) {
                                        config.setShowAvailableJobs(v);
                                    },
                                    "List available jobs");
        (void)calibration->add_option_function<std::string>("-j, --job-id",
                                                            [&config](const std::string& v) {
                                                                config.setJobId(v);
                                                            },
                                                            "Give a specific job ID to validate");

        // https://github.com/CLIUtils/CLI11/issues/88#issuecomment-978694416
        (void)calibration->require_option(1);

        return CalibrationCmds(*calibration, *mono, *stereo);
    }
} // namespace YACCP::CLI
