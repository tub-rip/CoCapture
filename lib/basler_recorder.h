#pragma once

#include <opencv2/opencv.hpp>
#include <pylon/BaslerUniversalInstantCamera.h>

namespace basler {

    class BaslerRecorder : public Pylon::CImageEventHandler {
    public:
        BaslerRecorder(std::string file_path, double fps) :
                file_path_(file_path),
                fps_(fps),
                video_writer_(NULL) {}

        ~BaslerRecorder() {
            delete video_writer_;
            Pylon::CImageEventHandler::~CImageEventHandler();
        }

    public:
        virtual void OnImageGrabbed(Pylon::CInstantCamera &camera,
                                    const Pylon::CGrabResultPtr &grab_result) {
            if (grab_result->GrabSucceeded()) {
                int width = grab_result->GetWidth();
                int height = grab_result->GetHeight();
                if (video_writer_ == NULL) {
                    video_writer_ = new cv::VideoWriter(file_path_,
                                                        cv::VideoWriter::fourcc('M', 'P', '4', 'V'),
                                                        fps_,
                                                        cv::Size(width, height),
                                                        0);
                }
                cv::Mat frame(height, width,
                              CV_8UC1, (uint8_t *) grab_result->GetBuffer());
                video_writer_->write(frame);
            } else {
                std::cout << "Error: " << std::hex << grab_result->GetErrorCode() <<
                          std::dec << " " << grab_result->GetErrorDescription() << std::endl;
            }
        }

    private:
        std::string file_path_;
        double fps_;

        cv::VideoWriter *video_writer_;
    };

}