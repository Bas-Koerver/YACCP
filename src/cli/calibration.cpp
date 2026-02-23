#include "calibration.hpp"

namespace YACCP::CLI {
    CalibrationCmds addCalibrationCmds(::CLI::App& app, CalibrationCmdConfig& config) {
        CalibrationCmds calibrationCmds;

        calibrationCmds.calibrate = app.add_subcommand("calibrate", "Global calibration command");

        calibrationCmds.calibrate->add_flag("-l, --list", config.showAvailableJobs, "List available jobs");
        calibrationCmds.calibrate->add_option("-j, --job-id", config.jobId, "Give a specific job ID to validate");

        calibrationCmds.calibrate->require_option(1);

        calibrationCmds.mono = calibrationCmds.calibrate->add_subcommand("mono", "Mono calibration");
        calibrationCmds.stereo = calibrationCmds.calibrate->add_subcommand("stereo", "stereo calibration");

        return calibrationCmds;
    }
} // namespace YACCP::CLI