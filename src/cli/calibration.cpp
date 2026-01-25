#include "calibration.hpp"

namespace YACCP::CLI {
    CalibrationCmds addCalibrationCmds(::CLI::App& app, CalibrationCmdConfig& config) {
        CalibrationCmds calibrationCmds;

        calibrationCmds.calibration = app.add_subcommand("calibrate", "Global calibration command");

        calibrationCmds.calibration->add_flag("-l, --list", config.showAvailableJobs, "List available jobs");
        calibrationCmds.calibration->add_option("-j, --job-id", config.jobId, "Give a specific job ID to validate");

        calibrationCmds.calibration->require_option(1);

        calibrationCmds.mono = calibrationCmds.calibration->add_subcommand("mono", "Mono calibration");
        calibrationCmds.stereo = calibrationCmds.calibration->add_subcommand("stereo", "stereo calibration");

        return calibrationCmds;
    }
} // namespace YACCP::CLI