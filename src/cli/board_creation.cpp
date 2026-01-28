#include "board_creation.hpp"

namespace YACCP::CLI {
    ::CLI::App& addBoardCreationCmd(::CLI::App& app, BoardCreationCmdConfig& config) {
        ::CLI::App* subCmd = app.add_subcommand("create-board", "Board creation");

        (void)subCmd->add_flag("-l, --list",
                               [&config](const bool& v) {
                                   config.setShowAvailableJobs(v);
                               },
                               "List jobs missing a board image and/or video");

        (void)subCmd->add_option_function<std::string>("-j, --job-id",
                                                       [&config](const std::string& v) {
                                                           config.setJobId(v);
                                                       },
                                                       "Give a specific job ID to create a board for.")
                    ->default_str("Creates a new job");

        (void)subCmd
              ->add_flag("!-i, !--image",
                         [&config](bool v) {
                             config.setGenerateImage(v);
                         },
                         "Whether to generate an image of the generated board")
              ->default_str("true");

        (void)subCmd
              ->add_flag("-v, --video",
                         [&config](bool v) {
                             config.setGenerateVideo(v);
                         },
                         "Whether to generate an event video of the generated board")
              ->default_str("false");

        return *subCmd;
    }
} // namespace YACCP::CLI
