#pragma once

#include <opencv2/opencv.hpp>
#include <pylon/BaslerUniversalInstantCamera.h>

namespace basler {

    class BaslerRecorder : public Pylon::CImageEventHandler {
    public:
        BaslerRecorder(std::string file_path, double fps, int width, int height) :
        fps_(fps),
        width_(width),
        height_(height),
        file_path_(file_path),
        is_recording_(false)
        {
            video_writer_ = new cv::VideoWriter(file_path_,
                                                cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                                fps_,
                                                cv::Size(width_, height_),
                                                0);
        }

        ~BaslerRecorder() override = default;

    public:
        virtual void OnImageGrabbed(Pylon::CInstantCamera &camera,
                                    const Pylon::CGrabResultPtr &grab_result) {
            if (grab_result->GrabSucceeded()) {
                if(is_recording_) {
                    cv::Mat frame(height_, width_,
                                  CV_8UC1, (uint8_t *) grab_result->GetBuffer());
                    video_writer_->write(frame);
                }
            } else {
                std::cout << "Error: " << std::hex << grab_result->GetErrorCode() <<
                          std::dec << " " << grab_result->GetErrorDescription() << std::endl;
            }
        }

        virtual void OnImageEventHandlerDeregistered(Pylon::CInstantCamera &camera) {
            video_writer_->release();
            delete this;
        }

        void start_recording() { is_recording_ = true; }
        void stop_recording() { is_recording_ = false; }

    private:
        std::string file_path_;
        double fps_;

        bool is_recording_;
        int width_;
        int height_;

        cv::VideoWriter *video_writer_;
    };

}