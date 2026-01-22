#include "board.hpp"

#include "orchestrator.hpp"

#include "../global_variables/config_defaults.hpp"

#include <boost/algorithm/string.hpp>

namespace YACCP::Config {
    BoardTypes stringToBoardType(std::string board) {
        boost::algorithm::to_lower(board);
        if (const auto it = boardTypesMap.find(board); it != boardTypesMap.end()) {
            return it->second;
        }
        throw std::runtime_error("board worker type: '" + board + "' does not exist.");
    }

    std::string boardTypeToString(const BoardTypes boardType) {
        for (const auto& [key, value] : boardTypesMap) {
            if (value == boardType) return key;
        }
        throw std::runtime_error("unknown board enum type");
    }

    void parseBoardConfig(const toml::table& tbl, BoardConfig& config, const bool boardCreation) {
        // [board] configuration variables.
        const auto* boardTbl{tbl["board"].as_table()};
        if (!boardTbl)
            throw std::runtime_error("Missing [board] table");

        const BoardTypes type{stringToBoardType(requireVariable<std::string>(*boardTbl, "type", "board"))};
        config.boardSize.width = (*boardTbl)["squares_x"].value_or(GlobalVariables::boardWidth);
        config.boardSize.height = (*boardTbl)["squares_y"].value_or(GlobalVariables::boardHeight);

        switch (type) {
        case BoardTypes::charuco:
            config.squarePixelLength = (*boardTbl)["square_pixel_length"].value_or(GlobalVariables::squarePixelLength);
            config.markerPixelLength = (*boardTbl)["marker_pixel_length"].value_or(GlobalVariables::markerPixelLength);
            config.marginSize = (*boardTbl)["margin_size"].value_or(config.squarePixelLength - config.markerPixelLength);
            config.borderBits = (*boardTbl)["border_bits"].value_or(GlobalVariables::borderBits);

            if (!boardCreation) {
                config.squareLength = requireVariable<float>(*boardTbl, "square_length", "board");
                if (config.squareLength <= 0)
                    throw std::runtime_error("square_length must be greater than zero");

                config.markerLength = requireVariable<float>(*boardTbl, "marker_length", "board");
                if (config.markerLength <= 0)
                    throw std::runtime_error("marker_length must be greater than zero");
            }
            break;
        }







    }
} // YACCP::Config