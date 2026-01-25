#include "orchestrator.hpp"


namespace YACCP::CLI {
    void addCli(CliCmdConfig& cliCmdConfig, CliCmds& cliCmds) {
        (void)cliCmds.app.description("Yet Another Camera Calibration Platform");
        (void)cliCmds.app.set_help_all_flag("--help-all", "Expanded help");
#ifdef NDEBUG
        cliCmds.app.require_subcommand(1);
#endif

        const auto formatter{cliCmds.app.get_formatter()};
        formatter->right_column_width(60);
        formatter->column_width(60);

        // Global CLI options and flags.
        (void)cliCmds.app.add_option("-p,--path",
                                     cliCmdConfig.appCmdConfig.userPath,
                                     "Path where config file is stored and where data directory will be placed")
                     ->check(::CLI::ExistingPath);

        // boardCreation CLI options
        cliCmds.boardCreationCmd = addBoardCreationCmd(cliCmds.app, cliCmdConfig.boardCreationCmdConfig);

        // Detection recording CLI options
        cliCmds.recordingCmd = addRecordingCmd(cliCmds.app, cliCmdConfig.recordingCmdConfig);

        // Image validation CLI options
        cliCmds.validationCmd = addValidationCmd(cliCmds.app, cliCmdConfig.validationCmdConfig);

        // Camera calibration CLI options
        cliCmds.calibrationCmds = addCalibrationCmds(cliCmds.app, cliCmdConfig.calibrationCmdConfig);
    }
} // YACCP::CLI