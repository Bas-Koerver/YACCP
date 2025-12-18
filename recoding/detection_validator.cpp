#include "detection_validator.hpp"

#include <stop_token>
#include <vector>

#include <opencv2/objdetect/charuco_detector.hpp>

#include "Utility.hpp"
#include "VideoViewer.hpp"

#include "recorders/camera_worker.hpp"

namespace YACCP {
    DetectionValidator::DetectionValidator(std::stop_source stopSource,
                                           std::vector<CamData> &camDatas,
                                           const cv::aruco::CharucoDetector &charucoDetector,
                                           moodycamel::ReaderWriterQueue<BoundingBoxData> &boundingBoxQ)
        : stopSource_(stopSource),
          stopToken_(stopSource.get_token()),
          camDatas_(camDatas),
          charucoDetector_(charucoDetector),
          boundingBoxQ_(boundingBoxQ) {
    }

    void DetectionValidator::start() {
        std::vector<VerifyTask> verifyTasks(camDatas_.size());
        std::vector<std::vector<cv::Point2f> > allCharucoCorners(camDatas_.size());
        cv::Size boardSize = charucoDetector_.getBoard().getChessboardSize();

        int cornerAmount = (boardSize.width - 1) * (boardSize.height - 1);

        while (!stopToken_.stop_requested()) {
            bool skipLoop{false};
            std::vector<int> camTaskCorrect{};
            bool taskIdsCorrect{false};

            // Keep dequeueing until all cameras have the same task id.
            do {
                // Get a new task from every camera, skip cameras that are already correct (have the highest taskID/frame index).
                for (auto i{0}; i < camDatas_.size(); ++i) {
                    if (std::find(camTaskCorrect.begin(), camTaskCorrect.end(), i) != camTaskCorrect.end()) {
                        continue;
                    }

                    camDatas_[i].frameVerifyQ.wait_dequeue(verifyTasks[i]);
                }

                // Get the largest task ID.
                int maxElement{};
                for (auto i{0}; i < camDatas_.size(); ++i) {
                    if (verifyTasks[i].id > maxElement) {
                        maxElement = verifyTasks[i].id;
                    }
                }

                // Check which cameras have the largest task ID.
                for (auto i{0}; i < camDatas_.size(); ++i) {
                    if (verifyTasks[i].id == maxElement) {
                        camTaskCorrect.emplace_back(i);
                    }
                }
                // If all cameras have the same task id, we are done and can continue with the detection.
                if (camTaskCorrect.size() == camDatas_.size()) {
                    taskIdsCorrect = true;
                }
            } while (!taskIdsCorrect);

            cv::Mat grayFrame;
            cv::cvtColor(verifyTasks[0].frame, grayFrame, cv::COLOR_BGR2GRAY);
            CharucoResults charucoResults = Utility::findBoard(charucoDetector_, grayFrame);
            if (!charucoResults.boardFound) {
                continue;
            }
            allCharucoCorners[0] = charucoResults.charucoCorners;
            std::vector<int> vec1{charucoResults.charucoIds};

            if (camDatas_.size() > 1) {
                std::vector<int> vec2;

                for (auto i{1}; i < camDatas_.size(); ++i) {
                    cv::cvtColor(verifyTasks[i].frame, grayFrame, cv::COLOR_BGR2GRAY);
                    charucoResults = Utility::findBoard(charucoDetector_, grayFrame);
                    if (!charucoResults.boardFound) {
                        continue;
                    }
                    allCharucoCorners[i] = charucoResults.charucoCorners;

                    vec2 = charucoResults.charucoIds;
                    vec1 = Utility::intersection(vec1, vec2);

                    if (static_cast<float>(vec1.size()) < std::floor(
                            static_cast<float>(cornerAmount) * intersectionMin_)) {
                        skipLoop = true;
                        break;
                    }
                }
            } else {
                if (static_cast<float>(vec1.size()) < std::floor(
                        static_cast<float>(cornerAmount) * intersectionMin_)) {
                    skipLoop = true;
                }
            }

            if (skipLoop) {
                continue;
            }

            for (auto i{0}; i < camDatas_.size(); ++i) {
                // Save image.
                // TODO: Make path configurable.
                std::string imagePath = "./data/images/cam_" + std::to_string(i) + "/";
                std::string imageName = "frame_" + std::to_string(verifyTasks[i].id) + ".png";
                Utility::createDirs(imagePath);

                cv::imwrite(imagePath + imageName, verifyTasks[i].frame);

                // Enqueue bounding box data for viewing.
                BoundingBoxData boundingBoxData;
                boundingBoxData.id = verifyTasks[i].id;
                boundingBoxData.camId = i;
                boundingBoxData.charucoIds = vec1;
                boundingBoxData.charucoCorners = allCharucoCorners[i];
                (void) boundingBoxQ_.enqueue(boundingBoxData);
            }
        }
    }
} // YACCP
