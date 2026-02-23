#include "board.hpp"

#include "orchestrator.hpp"

#include "../global_variables/config_defaults.hpp"

namespace YACCP::Config {
    void parseBoardConfig(const toml::table& tbl, BoardConfig& config, const bool boardCreation) {
        // [board] configuration variables.
        const auto* boardTbl{tbl["board"].as_table()};
        if (!boardTbl) throw std::runtime_error("Missing [board] table");

        config.boardType = stringToBoardType(requireVariable<std::string>(*boardTbl, "type", "board"));
        config.boardSize.width = (*boardTbl)["squares_x"].value_or(GlobalVariables::boardWidth);
        config.boardSize.height = (*boardTbl)["squares_y"].value_or(GlobalVariables::boardHeight);

        switch (config.boardType) {
        case BoardTypes::charuco:
            config.squarePixelLength = (*boardTbl)["square_pixel_length"].value_or(
                GlobalVariables::squarePixelLength);
            config.markerPixelLength = (*boardTbl)["marker_pixel_length"].value_or(GlobalVariables::markerPixelLength);
            config.marginSize = (*boardTbl)["margin_size"].
                value_or(config.squarePixelLength - config.markerPixelLength);
            config.borderBits = (*boardTbl)["border_bits"].value_or(GlobalVariables::borderBits);

            if (!boardCreation) {
                config.squareLength = requireVariable<float>(*boardTbl, "square_length", "board");
                if (config.squareLength <= 0) throw std::runtime_error("square_length must be greater than zero");

                config.markerLength = requireVariable<float>(*boardTbl, "marker_length", "board");
                if (config.markerLength <= 0) throw std::runtime_error("marker_length must be greater than zero");
            }
            break;
        }
    }
} // YACCP::Config
