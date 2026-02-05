#ifndef YACCP_SRC_CONFIG_DETECTION_HPP
#define YACCP_SRC_CONFIG_DETECTION_HPP
#include <nlohmann/json.hpp>

#include <opencv2/objdetect/charuco_detector.hpp>

#include <toml++/toml.hpp>

namespace cv::aruco {
    inline void to_json(nlohmann::json& j, const CharucoParameters& p) {
        j = {
            {"minMarkers", p.minMarkers},
            {"tryRefineMarkers", p.tryRefineMarkers}
        };
    }


    inline void from_json(const nlohmann::json& j, CharucoParameters& p) {
        (void)j.at("minMarkers").get_to(p.minMarkers);
        (void)j.at("tryRefineMarkers").get_to(p.tryRefineMarkers);
    }


    inline void to_json(nlohmann::json& j, const DetectorParameters& p) {
        j = {
            {"adaptiveThreshWinSizeMin", p.adaptiveThreshWinSizeMin},
            {"adaptiveThreshWinSizeMax", p.adaptiveThreshWinSizeMax},
            {"adaptiveThreshWinSizeStep", p.adaptiveThreshWinSizeStep},
            {"adaptiveThreshConstant", p.adaptiveThreshConstant},
            {"minMarkerPerimeterRate", p.minMarkerPerimeterRate},
            {"maxMarkerPerimeterRate", p.maxMarkerPerimeterRate},
            {"polygonalApproxAccuracyRate", p.polygonalApproxAccuracyRate},
            {"minCornerDistanceRate", p.minCornerDistanceRate},
            {"minDistanceToBorder", p.minDistanceToBorder},
            {"minMarkerDistanceRate", p.minMarkerDistanceRate},
            {"cornerRefinementMethod", p.cornerRefinementMethod},
            {"cornerRefinementWinSize", p.cornerRefinementWinSize},
            {"cornerRefinementMaxIterations", p.cornerRefinementMaxIterations},
            {"cornerRefinementMinAccuracy", p.cornerRefinementMinAccuracy},
            {"markerBorderBits", p.markerBorderBits},
            {"perspectiveRemovePixelPerCell", p.perspectiveRemovePixelPerCell},
            {"perspectiveRemoveIgnoredMarginPerCell", p.perspectiveRemoveIgnoredMarginPerCell},
            {"maxErroneousBitsInBorderRate", p.maxErroneousBitsInBorderRate},
            {"minOtsuStdDev", p.minOtsuStdDev},
            {"errorCorrectionRate", p.errorCorrectionRate},
            {"aprilTagQuadDecimate", p.aprilTagQuadDecimate},
            {"aprilTagQuadSigma", p.aprilTagQuadSigma},
            {"aprilTagMinClusterPixels", p.aprilTagMinClusterPixels},
            {"aprilTagMaxNmaxima", p.aprilTagMaxNmaxima},
            {"aprilTagCriticalRad", p.aprilTagCriticalRad},
            {"aprilTagMaxLineFitMse", p.aprilTagMaxLineFitMse},
            {"aprilTagMinWhiteBlackDiff", p.aprilTagMinWhiteBlackDiff},
            {"aprilTagDeglitch", p.aprilTagDeglitch},
            {"detectInvertedMarker", p.detectInvertedMarker},
            {"useAruco3Detection", p.useAruco3Detection},
            {"minSideLengthCanonicalImg", p.minSideLengthCanonicalImg},
            {"minMarkerLengthRatioOriginalImg", p.minMarkerLengthRatioOriginalImg}
        };
    }


    inline void from_json(const nlohmann::json& j, DetectorParameters& p) {
        (void)j.at("adaptiveThreshWinSizeMin").get_to(p.adaptiveThreshWinSizeMin);
        (void)j.at("adaptiveThreshWinSizeMax").get_to(p.adaptiveThreshWinSizeMax);
        (void)j.at("adaptiveThreshWinSizeStep").get_to(p.adaptiveThreshWinSizeStep);
        (void)j.at("adaptiveThreshConstant").get_to(p.adaptiveThreshConstant);
        (void)j.at("minMarkerPerimeterRate").get_to(p.minMarkerPerimeterRate);
        (void)j.at("maxMarkerPerimeterRate").get_to(p.maxMarkerPerimeterRate);
        (void)j.at("polygonalApproxAccuracyRate").get_to(p.polygonalApproxAccuracyRate);
        (void)j.at("minCornerDistanceRate").get_to(p.minCornerDistanceRate);
        (void)j.at("minDistanceToBorder").get_to(p.minDistanceToBorder);
        (void)j.at("minMarkerDistanceRate").get_to(p.minMarkerDistanceRate);
        (void)j.at("cornerRefinementMethod").get_to(p.cornerRefinementMethod);
        (void)j.at("cornerRefinementWinSize").get_to(p.cornerRefinementWinSize);
        (void)j.at("cornerRefinementMaxIterations").get_to(p.cornerRefinementMaxIterations);
        (void)j.at("cornerRefinementMinAccuracy").get_to(p.cornerRefinementMinAccuracy);
        (void)j.at("markerBorderBits").get_to(p.markerBorderBits);
        (void)j.at("perspectiveRemovePixelPerCell").get_to(p.perspectiveRemovePixelPerCell);
        (void)j.at("perspectiveRemoveIgnoredMarginPerCell").get_to(p.perspectiveRemoveIgnoredMarginPerCell);
        (void)j.at("maxErroneousBitsInBorderRate").get_to(p.maxErroneousBitsInBorderRate);
        (void)j.at("minOtsuStdDev").get_to(p.minOtsuStdDev);
        (void)j.at("errorCorrectionRate").get_to(p.errorCorrectionRate);
        (void)j.at("aprilTagQuadDecimate").get_to(p.aprilTagQuadDecimate);
        (void)j.at("aprilTagQuadSigma").get_to(p.aprilTagQuadSigma);
        (void)j.at("aprilTagMinClusterPixels").get_to(p.aprilTagMinClusterPixels);
        (void)j.at("aprilTagMaxNmaxima").get_to(p.aprilTagMaxNmaxima);
        (void)j.at("aprilTagCriticalRad").get_to(p.aprilTagCriticalRad);
        (void)j.at("aprilTagMaxLineFitMse").get_to(p.aprilTagMaxLineFitMse);
        (void)j.at("aprilTagMinWhiteBlackDiff").get_to(p.aprilTagMinWhiteBlackDiff);
        (void)j.at("aprilTagDeglitch").get_to(p.aprilTagDeglitch);
        (void)j.at("detectInvertedMarker").get_to(p.detectInvertedMarker);
        (void)j.at("useAruco3Detection").get_to(p.useAruco3Detection);
        (void)j.at("minSideLengthCanonicalImg").get_to(p.minSideLengthCanonicalImg);
        (void)j.at("minMarkerLengthRatioOriginalImg").get_to(p.minMarkerLengthRatioOriginalImg);
    }
}

namespace YACCP::Config {
    struct DetectionConfig {
        int openCvArucoDictionaryId{};
        float cornerMin{};

        cv::aruco::CharucoParameters charucoParameters{};
        cv::aruco::DetectorParameters detectorParameters{};
    };


    inline void to_json(nlohmann::json& j, const DetectionConfig& d) {
        j = {
            {"openCvDictionaryId", d.openCvArucoDictionaryId},
            {"cornerMin", d.cornerMin},
            {"charucoParameters", d.charucoParameters},
            {"detectorParameters", d.detectorParameters},
        };
    }


    inline void from_json(const nlohmann::json& j, DetectionConfig& d) {
        (void)j.at("openCvDictionaryId").get_to(d.openCvArucoDictionaryId);
        (void)j.at("cornerMin").get_to(d.cornerMin);
        (void)j.at("charucoParameters").get_to(d.charucoParameters);
        (void)j.at("detectorParameters").get_to(d.detectorParameters);
    }


    void parseDetectionConfig(const toml::table& tbl, DetectionConfig& config);
} // YACCP::Config

#endif //YACCP_SRC_CONFIG_DETECTION_HPP