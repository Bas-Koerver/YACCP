#include "board.hpp"

#include <stdexcept>

#include "orchestrator.hpp"
#include "../defaults.hpp"

namespace YACCP::Config {
    void parseBoardConfig(const toml::table& tbl, BoardConfig& config) {
        // [board] configuration variables.
        const auto* boardTbl{tbl["board"].as_table()};
        if (!boardTbl)
            throw std::runtime_error("Missing [board] table");

        config.boardSize.width = (*boardTbl)["squares_x"].value_or(Defaults::boardWidth);
        config.boardSize.height = (*boardTbl)["squares_y"].value_or(Defaults::boardHeight);

        config.squareLength = requireVariable<float>(*boardTbl, "square_length", "board");
        config.markerLength = requireVariable<float>(*boardTbl, "marker_length", "board");
    }
} // YACCP::Config
