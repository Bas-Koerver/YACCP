#define NOMINMAX
#ifdef NDEBUG
#include <opencv2/core/utils/logger.hpp>
#endif

#include "utility.hpp"

#include "executors/board_runner.hpp"
#include "executors/calibration_runner.hpp"
#if YACCP_HAS_METAVISION
#include "executors/recording_runner.hpp"
#include "executors/validation_runner.hpp"
#endif

#include <CLI/App.hpp>
#include <GLFW/glfw3.h>


int main(int argc, char** argv) {
    // Decrease log level to warning for release builds.
#ifdef NDEBUG
    cv::utils::logging::setLogLevel(cv::utils::logging::LogLevel::LOG_LEVEL_WARNING);
#endif
    // Exitcode guidelines: https://tldp.org/LDP/abs/html/exitcodes.html
    auto exitCode{0};

    // Try to initialise GLFW
    if (glfwInit() == GLFW_FALSE) {
        std::cerr << "Failed to initialise GLFW.\n";
        exitCode = 1;
    }

    // @brief CLI
    YACCP::CLI::CliCmdConfig cliCmdConfig;
    YACCP::CLI::CliCmds cliCmds;
    YACCP::CLI::addCli(cliCmdConfig, cliCmds);
    CLI11_PARSE(cliCmds.app, argc, argv);

    /**
    * Global variable declaration
    */
    std::string dateTime;
    dateTime = YACCP::Utility::getCurrentDateTime();
    auto workingDir = std::filesystem::current_path();

    std::filesystem::path path = workingDir / cliCmdConfig.appCmdConfig.userPath;

    if (exitCode == 0) {
        if (*cliCmds.boardCreationCmd) {
            try {
                exitCode = YACCP::Executor::runBoardCreation(cliCmdConfig, path, dateTime);
            }
            catch (const std::exception& err) {
                std::cerr << err.what() << "\n";
            }
#if YACCP_HAS_METAVISION
        } else if (*cliCmds.recordingCmds.record) {
                try {
                    exitCode = YACCP::Executor::runRecording(cliCmdConfig, cliCmds, path, dateTime);
                }
                catch (const std::exception& err) {
                    std::cerr << err.what() << "\n";
                    exitCode = 1;
                }
            } else if (*cliCmds.validationCmd) {
                try {
                    YACCP::Executor::runValidation(cliCmdConfig, path, dateTime);
                }
                catch (const std::exception& err) {
                    std::cerr << err.what() << "\n";
                }
#endif
        } else if (*cliCmds.calibrationCmds.calibrate) {
            try {
                YACCP::Executor::runCalibration(cliCmdConfig, cliCmds, path, dateTime);
            }
            catch (const std::exception& err) {
                std::cerr << err.what() << "\n";
            }
        } else {
            std::cerr << "No valid sub command given\n";
        }
    }

    return exitCode;
}