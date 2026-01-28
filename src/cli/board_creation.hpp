#ifndef YACCP_SRC_CLI_BOARD_CREATION_HPP
#define YACCP_SRC_CLI_BOARD_CREATION_HPP
#include "../global_variables/cli_defaults.hpp"

#include <CLI/App.hpp>


namespace YACCP::CLI {
    class BoardCreationCmdConfig {
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


        [[nodiscard]] bool generateImage() const {
            return generateImage_;
        }


        void setGenerateImage(const bool generateImage) {
            generateImage_ = generateImage;
        }


        [[nodiscard]] bool generateVideo() const {
            return generateVideo_;
        }


        void setGenerateVideo(const bool generateVideo) {
            generateVideo_ = generateVideo;
        }


    private:
        bool showAvailableJobs_{};
        std::string jobId_{};
        bool generateImage_{GlobalVariables::generateImage};
        bool generateVideo_{GlobalVariables::generateVideo};
    };

    ::CLI::App& addBoardCreationCmd(::CLI::App& app, BoardCreationCmdConfig& config);
} // namespace YACCP::CLI

#endif // YACCP_SRC_CLI_BOARD_CREATION_HPP
