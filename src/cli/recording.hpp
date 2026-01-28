#ifndef YACCP_SRC_CLI_RECORDING_HPP
#define YACCP_SRC_CLI_RECORDING_HPP
#include <CLI/App.hpp>

namespace YACCP::CLI {
    class RecordingCmdConfig {
    public:
        [[nodiscard]] bool showAvailableJobs() const {
            return showAvailableJobs_;
        }


        void setShowAvailableJobs(const bool showAvailableJobs) {
            showAvailableJobs_ = showAvailableJobs;
        }


        [[nodiscard]] const std::string& jobId() const {
            return jobId_;
        }


        void setJobId(std::string jobId) {
            jobId_ = std::move(jobId);
        }


        [[nodiscard]] bool showAvailableCams() const {
            return showAvailableCams_;
        }


        void setShowAvailableCams(const bool showAvailableCams) {
            showAvailableCams_ = showAvailableCams;
        }


    private:
        bool showAvailableJobs_{};
        std::string jobId_{};
        bool showAvailableCams_{};
    };


    ::CLI::App& addRecordingCmd(::CLI::App& app, RecordingCmdConfig& config);
} // namespace YACCP::CLI

#endif // YACCP_SRC_CLI_RECORDING_HPP
