#ifdef ENABLE_PYLON_SDK
#ifndef RIP_COCAPTURE_GUI_IMAGE_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_IMAGE_EVENT_HANDLER_H

#include <opencv2/opencv.hpp>
#include <pylon/PylonIncludes.h>

namespace rcg::cams::basler {

    class ImageEventHandler : public Pylon::CImageEventHandler {
    public:
        ImageEventHandler() : first_image_grabbed_(false), image_available_(false), is_recording_(false) {}
        ~ImageEventHandler() {}

    public:
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

                if(is_recording_) {
                    video_writer_.write(image_frame);
                }

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

        void StartRecording(const char* output_dir) {
            image_event_handler_mutex_.lock();

            std::string output_dir_str {output_dir};
            if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }

            video_writer_ = cv::VideoWriter {output_dir_str + "/frames.mp4",
                                             cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                             30,
                                             cv::Size(dimensions_.second, dimensions_.first),
                                             false};
            is_recording_ = true;

            image_event_handler_mutex_.unlock();
        }

        void StopRecording() {
            image_event_handler_mutex_.lock();

            is_recording_ = false;
            video_writer_.release();

            image_event_handler_mutex_.unlock();
        }

    private:
        bool first_image_grabbed_;
        std::pair<int, int> dimensions_;

        cv::Mat image_frame_;
        bool image_available_;
        std::mutex image_event_handler_mutex_;

        cv::VideoWriter video_writer_;
        bool is_recording_;
    };

} // rcg::cams::basler

#endif //RIP_COCAPTURE_GUI_IMAGE_EVENT_HANDLER_H
#endif //ENABLE_PYLON_SDK