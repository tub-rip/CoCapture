#ifndef COCAPTURE_CAMERA_INTERFACE_H
#define COCAPTURE_CAMERA_INTERFACE_H

#include <string>

#include <opencv2/opencv.hpp>

#include "camera.h"

namespace Cocapture {

    class ICamera {
    public:
        virtual void Start() {}
        virtual std::string GetInfo(int info) { return {}; }
        virtual double GetProp(int property) { return {}; }
        virtual void SetProp(int property, double value) {}
        virtual void AcquireFrame(int) {}
        virtual void OutputFrame(int, cv::Mat&) = 0;
        virtual void StartRecording(std::string) {}
        virtual void StopRecording() {}
        virtual void Clean() {}
    };

} // Cocapture

#endif //COCAPTURE_CAMERA_INTERFACE_H
