#ifndef YACCP_DEFAULTS_HPP
#define YACCP_DEFAULTS_HPP

namespace YACCP::Defaults {
    // TOML defaults
    // inline constexpr std::string_view boardConfigTbl{"board"};
    // inline constexpr std::string_view detectionConfigTbl{"detection"};
    // inline constexpr std::string_view recordingConfigTbl{"recording"};
    // inline constexpr std::string_view viewingConfigTbl{"viewing"};
    // inline constexpr std::string_view calibrationConfigTbl{"calibration"};

    inline constexpr auto boardWidth{7}; // Amount of squares
    inline constexpr auto boardHeight{5};

    inline constexpr auto charucoDictionary{8};
    inline constexpr auto detectionInterval{2}; // seconds
    inline constexpr auto minCornerFraction{.125F};

    inline constexpr auto recordingFps{30};
    inline constexpr auto accumulationTime{33333};

    inline constexpr auto camViewsHorizontal{3};

} // YACCP::Defaults

#endif //YACCP_DEFAULTS_HPP