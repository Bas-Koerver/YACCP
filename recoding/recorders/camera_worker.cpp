//
// Created by Bas_K on 2025-12-08.
//

#include "camera_worker.hpp"

#include <iostream>
#include <stop_token>

namespace YACCP {
    CameraWorker::CameraWorker(std::stop_source stopSource,
                               std::vector<YACCP::CamData> &camDatas,
                               int fps,
                               int id,
                               std::string camId)
        : stopSource_(stopSource),
          stopToken_(stopSource.get_token()),
          camDatas_(camDatas),
          camData_(camDatas.at(id)),
          fps_(fps),
          id_(id),
          camId_(std::move(camId)) {
    }

    void CameraWorker::listAvailableSources() {
        std::cerr << "The function to list the available sources is not implemented \n";
        throw std::logic_error("Function not yet implemented");
    }

    void CameraWorker::start() {
        std::cerr << "The function to start the camera worker is not implemented \n";
        throw std::logic_error("Function not yet implemented");
    }
} // YACCP
