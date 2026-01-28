#ifndef YACCP_SRC_CLI_VALIDATION_HPP
#define YACCP_SRC_CLI_VALIDATION_HPP
#include <CLI/App.hpp>

namespace YACCP::CLI {
    class ValidationCmdConfig {
    public:
        [[nodiscard]] bool showAvailableJobs() const {
            return showAvailableJobs_;
        }


        void setShowAvailableJobs(bool showAvailableJobs) {
            showAvailableJobs_ = showAvailableJobs;
        }


        [[nodiscard]] const std::string& jobId() const {
            return jobId_;
        }


        void setJobId(std::string jobId) {
            jobId_ = std::move(jobId);
        }


    private:
        bool showAvailableJobs_{};
        std::string jobId_{};
    };

    ::CLI::App& addValidationCmd(::CLI::App& app, ValidationCmdConfig& config);
} // namespace YACCP::CLI

#endif // YACCP_SRC_CLI_VALIDATION_HPP
