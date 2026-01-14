#include <filesystem>

#include <opencv2/core/mat.hpp>
#include <opencv2/objdetect/aruco_detector.hpp>
#include <opencv2/objdetect/charuco_detector.hpp>

#include "utility.hpp"

#include <iostream>

namespace YACCP::Utility {
    void clearScreen() {
        // "\x1b" Escape character
        // "[2J" Clears entire screen
        // "[1;1H" Moves cursor to the top-left corner
        // https://medium.com/@ryan_forrester_/c-screen-clearing-how-to-guide-cff5bf764ccd
        std::cout << "\x1b[2J\x1b[1;1H" << std::flush;
    }

    // MISRA deviation: OpenCV Charuco API requires std::vector
    CharucoResults findBoard(const cv::aruco::CharucoDetector& charucoDetector,
                             const cv::Mat& gray,
                             const int cornerMin) {
        CharucoResults charucoResults;

        charucoDetector.detectBoard(gray,
                                    charucoResults.charucoCorners,
                                    charucoResults.charucoIds,
                                    charucoResults.markerCorners,
                                    charucoResults.markerIds);

        if (charucoResults.charucoCorners.size() > cornerMin) {
            charucoResults.boardFound = true;
        }
        return charucoResults;
    }

    std::_Timeobj<char, const tm*> getCurrentDateTime() {
        // Get the current date and time.
        const auto now = std::chrono::system_clock::now();
        const auto localTime = std::chrono::system_clock::to_time_t(now);

        // Generate timestamp at UTC +0
        return std::put_time(std::gmtime(&localTime), "%F_%H-%M-%S");
    }

    bool isNonEmptyDirectory(const std::filesystem::path& path) {
        return std::filesystem::exists(path) &&
            std::filesystem::is_directory(path) &&
            !std::filesystem::is_empty(path);
    }

    nlohmann::json loadJsonFromFile(std::ifstream& file) {
        nlohmann::json j;
        try {
            nlohmann::json j{nlohmann::json::parse(file)};
        } catch (const nlohmann::json::parse_error& e) {
            std::stringstream ss{};
            ss << "There is a problem with the job_data.json\n" << e.what();
            throw std::runtime_error(ss.str());
        }

        return j;
    }

    std::ifstream openFile(const std::filesystem::path& path, const std::string& fileName) {
        const auto filePath = path / fileName;

        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file " + filePath.string());
        }

        return file;
    }






} // YACCP::Utility
