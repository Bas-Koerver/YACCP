#include "orchestrator.hpp"


namespace YACCP::CLI {
    CliCmds::CliCmds(::CLI::App& app,
                     ::CLI::App& boardCreationCmd,
                     ::CLI::App& recordingCmd,
                     ::CLI::App& validationCmd,
                     const CalibrationCmds& calibrationCmds) noexcept :
        app_(app),
        boardCreationCmd_(boardCreationCmd),
        recordingCmd_(recordingCmd),
        validationCmd_(validationCmd),
        calibrationCmds_(calibrationCmds) {

    }


    CliCmds addCli(CliCmdConfig& cliCmdConfig, ::CLI::App& app) {
        (void)app.description("Yet Another Camera Calibration Platform");
        (void)app.set_help_all_flag("--help-all", "Expanded help");
#ifdef NDEBUG
        (void)app.require_subcommand(1);
#endif

        const auto formatter{app.get_formatter()};
        formatter->right_column_width(60);
        formatter->column_width(60);

        // Global CLI options and flags.
        (void)app.add_option_function<std::filesystem::path>("-p,--path",
                                                             [&cliCmdConfig](const std::filesystem::path& v) {
                                                                 cliCmdConfig.appCmdConfig().setUserPath(v);
                                                             },
                                                             "Path where config file is stored and where data directory will be placed")
                 ->check(::CLI::ExistingPath);

        // boardCreation CLI options
        ::CLI::App& boardCreationCmd = addBoardCreationCmd(app, cliCmdConfig.boardCreationCmdConfig());

        // Detection recording CLI options
        ::CLI::App& recordingCmd = addRecordingCmd(app, cliCmdConfig.recordingCmdConfig());

        // Image validation CLI options
        ::CLI::App& validationCmd = addValidationCmd(app, cliCmdConfig.validationCmdConfig());

        // Camera calibration CLI options
        CalibrationCmds calibrationCmds = addCalibrationCmds(app, cliCmdConfig.calibrationCmdConfig());

        return CliCmds(app, boardCreationCmd, recordingCmd, validationCmd, std::move(calibrationCmds));
    }
} // YACCP::CLI
