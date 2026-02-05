#include "board_creation.hpp"

namespace YACCP::CLI {
    ::CLI::App* addBoardCreationCmd(::CLI::App& app, BoardCreationCmdConfig& config) {
        ::CLI::App* subCmd = app.add_subcommand("create-board", "Board creation");

        subCmd->add_flag("-l, --list", config.showAvailableJobs, "List jobs missing a board image and/or video");

        subCmd->add_option("-j, --job-id", config.jobId, "Give a specific job ID to create a board for.");

        subCmd
            ->add_flag("!-i, !--image",
                       config.generateImage,
                       "Whether to generate an image of the generated board")
            ->default_str("true");

        subCmd
            ->add_flag("-v, --video",
                       config.generateVideo,
                       "Whether to generate an event video of the generated board")
            ->default_str("false");

        return subCmd;
    }
} // namespace YACCP::CLI