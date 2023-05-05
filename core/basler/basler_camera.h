#ifdef ENABLE_PYLON_SDK
#ifndef COCAPTURE_BASLER_CAMERA_H
#define COCAPTURE_BASLER_CAMERA_H

#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "camera_interface.h"

namespace Cocapture {

    class BaslerCamera : public ICamera {
    public:
        BaslerCamera(std::string serial_number);

    public:
        void Start() override;
        std::string GetInfo(int info) override;
        double GetProp(int property) override;
        void SetProp(int property, double value) override;
        void OutputFrame(int, cv::Mat& image_frame) override;
        void StartRecording(std::string output_directory_path) override;
        void StopRecording() override;
        void Clean() override;

    public:
        static Pylon::String_t ToString_t(int value);
        static int ToEnum(Pylon::String_t value);

    private:
        class ImageEventHandler : public Pylon::CImageEventHandler {
        public:
            ImageEventHandler() : first_image_grabbed_(false), image_available_(false), is_recording_(false) {}

            void OnImageGrabbed(Pylon::CInstantCamera& camera, const Pylon::CGrabResultPtr& grabResult) {
                if(grabResult->GrabSucceeded()) {
                    image_event_handler_mutex_.lock();

                    if(!first_image_grabbed_) {
                        dimensions_.first = grabResult->GetHeight();
                        dimensions_.second = grabResult->GetWidth();
                        first_image_grabbed_ = true;
                    }

                    cv::Mat image_frame {dimensions_.first, dimensions_.second,
                                         CV_8UC1, grabResult->GetBuffer()};
                    if(is_recording_) { video_writer_->write(image_frame); }
                    image_frame.copyTo(image_frame_);
                    image_available_ = true;

                    image_event_handler_mutex_.unlock();
                }
            }

            void OutputFrame(cv::Mat& image_frame) {
                image_event_handler_mutex_.lock();

                if(image_available_) {
                    cv::cvtColor(image_frame_, image_frame, cv::COLOR_GRAY2RGB);
                    image_available_ = false;
                }

                image_event_handler_mutex_.unlock();
            }

            void StartRecording(std::string output_directory_path) {
                image_event_handler_mutex_.lock();

                if(output_directory_path.back() == '/') { output_directory_path.pop_back(); }
                video_writer_ = new cv::VideoWriter {output_directory_path + "/frames.mp4",
                                                     cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                                     30,
                                                     cv::Size {dimensions_.second, dimensions_.first},
                                                     false};
                is_recording_ = true;

                image_event_handler_mutex_.unlock();
            }

            void StopRecording() {
                image_event_handler_mutex_.lock();

                is_recording_ = false;
                video_writer_->release();

                image_event_handler_mutex_.unlock();
            }

        private:
            bool first_image_grabbed_;
            std::pair<int, int> dimensions_;

            cv::Mat image_frame_;
            bool image_available_;
            std::mutex image_event_handler_mutex_;

            cv::VideoWriter* video_writer_;
            bool is_recording_;
        };

    private:
        Pylon::CBaslerUniversalInstantCamera camera_;
        ImageEventHandler* image_event_handler_;
    };

} // Cocapture

#endif //COCAPTURE_BASLER_CAMERA_H
#endif //ENABLE_PYLON_SDK