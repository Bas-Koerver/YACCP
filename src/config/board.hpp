#ifndef YACCP_SRC_CONFIG_BOARD_HPP
#define YACCP_SRC_CONFIG_BOARD_HPP
#include <boost/algorithm/string/case_conv.hpp>
#include <nlohmann/json.hpp>
#include <opencv2/core/types.hpp>
#include <toml++/toml.hpp>

namespace YACCP::Config {
    /**
    * @brief Simple enum to represent different board types.
    */
    enum class BoardTypes {
        charuco,
    };

    inline std::unordered_map<std::string, BoardTypes> boardTypesMap{
        {"charuco", BoardTypes::charuco},
    };


    inline BoardTypes stringToBoardType(std::string board) {
        boost::algorithm::to_lower(board);
        if (const auto it = boardTypesMap.find(board); it != boardTypesMap.end()) {
            return it->second;
        }
        throw std::runtime_error("board worker type: '" + board + "' does not exist.");
    }


    inline std::string boardTypeToString(const BoardTypes boardType) {
        for (const auto& [key, value] : boardTypesMap) {
            if (value == boardType) {
                return key;
            }
        }
        throw std::runtime_error("unknown board enum type");
    }


    struct BoardConfig {
        BoardTypes boardType{};
        cv::Size boardSize{};
        float squareLength{};
        float markerLength{};
        int squarePixelLength{};
        int markerPixelLength{};
        int marginSize{};
        int borderBits{};
    };

    void parseBoardConfig(const toml::table& tbl, BoardConfig& config, bool boardCreation);


    inline void to_json(nlohmann::json& j, const BoardConfig& b) {
        j = {
            {"boardType", boardTypeToString(b.boardType)},
            {
                "boardSize",
                {
                    {"width", b.boardSize.width},
                    {"height", b.boardSize.height},
                }
            },
            {"squareLength", b.squareLength},
            {"markerLength", b.markerLength},
            {"squarePixelLength", b.squarePixelLength},
            {"markerPixelLength", b.markerPixelLength},
            {"marginSize", b.marginSize},
            {"borderBits", b.borderBits}
        };
    }


    inline void from_json(const nlohmann::json& j, BoardConfig& b) {
        b.boardType = stringToBoardType(j.at("boardType").get<std::string>());
        (void)j.at("boardSize").at("width").get_to(b.boardSize.width);
        (void)j.at("boardSize").at("height").get_to(b.boardSize.height);
        (void)j.at("squareLength").get_to(b.squareLength);
        (void)j.at("markerLength").get_to(b.markerLength);
        (void)j.at("squarePixelLength").get_to(b.squarePixelLength);
        (void)j.at("markerPixelLength").get_to(b.markerPixelLength);
        (void)j.at("marginSize").get_to(b.marginSize);
        (void)j.at("borderBits").get_to(b.borderBits);
    }
} // YACCP::Config

#endif //YACCP_SRC_CONFIG_BOARD_HPP
