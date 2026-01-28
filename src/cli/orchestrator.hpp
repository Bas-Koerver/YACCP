#ifndef YACCP_SRC_CLI_ORCHESTRATOR_HPP
#define YACCP_SRC_CLI_ORCHESTRATOR_HPP
#include "board_creation.hpp"
#include "calibration.hpp"
#include "recording.hpp"
#include "validation.hpp"


namespace YACCP::CLI {
    class AppCmdConfig {
    public:
        [[nodiscard]] const std::filesystem::path& userPath() const {
            return userPath_;
        }


        void setUserPath(std::filesystem::path userPath) {
            userPath_ = std::move(userPath);
        }


    private:
        std::filesystem::path userPath_;
    };

    class CliCmdConfig {
    public:
        [[nodiscard]] AppCmdConfig& appCmdConfig() {
            return appCmdConfig_;
        }

        [[nodiscard]] const AppCmdConfig& appCmdConfig() const {
            return appCmdConfig_;
        }


        void setAppCmdConfig(AppCmdConfig appCmdConfig) {
            appCmdConfig_ = std::move(appCmdConfig);
        }


        [[nodiscard]] BoardCreationCmdConfig& boardCreationCmdConfig() {
            return boardCreationCmdConfig_;
        }


        [[nodiscard]] const BoardCreationCmdConfig& boardCreationCmdConfig() const {
            return boardCreationCmdConfig_;
        }


        void setBoardCreationCmdConfig(BoardCreationCmdConfig boardCreationCmdConfig) {
            boardCreationCmdConfig_ = std::move(boardCreationCmdConfig);
        }


        [[nodiscard]] RecordingCmdConfig& recordingCmdConfig() {
            return recordingCmdConfig_;
        }


        [[nodiscard]] const RecordingCmdConfig& recordingCmdConfig() const {
            return recordingCmdConfig_;
        }


        void setRecordingCmdConfig(RecordingCmdConfig recordingCmdConfig) {
            recordingCmdConfig_ = std::move(recordingCmdConfig);
        }


        [[nodiscard]] ValidationCmdConfig& validationCmdConfig() {
            return validationCmdConfig_;
        }


        [[nodiscard]] const ValidationCmdConfig& validationCmdConfig() const {
            return validationCmdConfig_;
        }


        void setValidationCmdConfig(ValidationCmdConfig validationCmdConfig) {
            validationCmdConfig_ = std::move(validationCmdConfig);
        }


        [[nodiscard]] CalibrationCmdConfig& calibrationCmdConfig() {
            return calibrationCmdConfig_;
        }


        [[nodiscard]] const CalibrationCmdConfig& calibrationCmdConfig() const {
            return calibrationCmdConfig_;
        }


        void setCalibrationCmdConfig(CalibrationCmdConfig calibrationCmdConfig) {
            calibrationCmdConfig_ = std::move(calibrationCmdConfig);
        }


    private:
        AppCmdConfig appCmdConfig_{};
        BoardCreationCmdConfig boardCreationCmdConfig_{};
        RecordingCmdConfig recordingCmdConfig_{};
        ValidationCmdConfig validationCmdConfig_{};
        CalibrationCmdConfig calibrationCmdConfig_{};
    };

    class CliCmds {
    public:
        [[nodiscard]] ::CLI::App& app() noexcept {
            return app_;
        }

        [[nodiscard]] const ::CLI::App& app() const noexcept {
            return app_;
        }


        [[nodiscard]] const ::CLI::App& boardCreationCmd() const noexcept {
            return boardCreationCmd_;
        }


        [[nodiscard]] const ::CLI::App& recordingCmd() const noexcept {
            return recordingCmd_;
        }


        [[nodiscard]] const ::CLI::App& validationCmd() const noexcept {
            return validationCmd_;
        }


        [[nodiscard]] const CalibrationCmds& calibrationCmds() const noexcept {
            return calibrationCmds_;
        }


        CliCmds(::CLI::App& app,
                ::CLI::App& boardCreationCmd,
                ::CLI::App& recordingCmd,
                ::CLI::App& validationCmd,
                const CalibrationCmds& calibrationCmds) noexcept;

    private:
        ::CLI::App& app_;
        ::CLI::App& boardCreationCmd_;
        ::CLI::App& recordingCmd_;
        ::CLI::App& validationCmd_;
        CalibrationCmds calibrationCmds_;
    };

    CliCmds addCli(CliCmdConfig& cliCmdConfig, ::CLI::App& app);
} // YACCP::CLI


#endif //YACCP_SRC_CLI_ORCHESTRATOR_HPP
