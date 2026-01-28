#ifndef YACCP_SRC_CLI_CALIBRATION_HPP
#define YACCP_SRC_CLI_CALIBRATION_HPP
#include <CLI/App.hpp>

namespace YACCP::CLI {
    class CalibrationCmdConfig {
    public:
        [[nodiscard]] bool showAvailableJobs1() const {
            return showAvailableJobs_;
        }


        void setShowAvailableJobs(bool showAvailableJobs) {
            this->showAvailableJobs_ = showAvailableJobs;
        }


        [[nodiscard]] const std::string& jobId1() const {
            return jobId_;
        }


        void setJobId(std::string jobId) {
            this->jobId_ = std::move(jobId);
        }


    private:
        bool showAvailableJobs_{};
        std::string jobId_{};
    };

    class CalibrationCmds {
    public:
        CalibrationCmds(::CLI::App& calibration, ::CLI::App& mono, ::CLI::App& stereo) noexcept;


        [[nodiscard]] ::CLI::App& calibration() const noexcept {
            return calibration_;
        }


        [[nodiscard]] ::CLI::App& mono() const noexcept {
            return mono_;
        }


        [[nodiscard]] ::CLI::App& stereo() const noexcept {
            return stereo_;
        }


    private:
        ::CLI::App& calibration_;
        ::CLI::App& mono_;
        ::CLI::App& stereo_;
    };

    CalibrationCmds addCalibrationCmds(::CLI::App& app, CalibrationCmdConfig& config);
} // namespace YACCP::CLI

#endif // YACCP_SRC_CLI_CALIBRATION_HPP
