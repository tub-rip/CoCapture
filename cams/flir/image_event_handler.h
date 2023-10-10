#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H

#include <filesystem>

namespace rcg::cams::flir {
    class ImageEventHandlerBase : public Spinnaker::ImageEventHandler {
    public:
        ImageEventHandlerBase(int height, int width) :
                frame_received_(false),
                height_(height),
                width_(width),
                is_recording_(false) {}

        virtual void StartRecording(const char* output_dir) = 0;
        virtual void StopRecording() = 0;
        virtual void WriteFrame(Spinnaker::ImagePtr& frame) = 0;
        virtual void AnalyzeRecording(const char* output_dir) = 0;

        void OnImageEvent(Spinnaker::ImagePtr image) {
            std::unique_lock<std::mutex> lock(frame_mutex_);

            if (is_recording_) {
                this->WriteFrame(image);
            }
            frame_ = image;
            frame_received_ = true;
        }

        void OutputFrame(cv::Mat& output_frame) {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            if (frame_received_) {
                Spinnaker::ImagePtr tmp = processor_.Convert(frame_,
                                                             Spinnaker::PixelFormat_RGB8);
                output_frame = cv::Mat(height_, width_, CV_8UC3, tmp->GetData());
            } else {
                output_frame = cv::Mat::zeros(height_, width_, CV_8UC3);
            }
        }

    protected:
        Spinnaker::ImagePtr frame_;
        std::mutex frame_mutex_;
        bool frame_received_;
        int height_, width_;
        bool is_recording_;
        Spinnaker::ImageProcessor processor_;
    };

    //class Mp4ImageEventHandler : public ImageEventHandlerBase {
    //public:
    //    Mp4ImageEventHandler(int height, int width) : ImageEventHandlerBase(height, width) {}
//
    //    void StartRecording(const char* output_dir) {
    //        std::unique_lock<std::mutex> lock(frame_mutex_);
//
    //        std::string output_dir_str {output_dir};
    //        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }
//
    //        video_writer_ = cv::VideoWriter {output_dir_str + "/frames.mp4",
    //                                         cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
    //                                         30,
    //                                         cv::Size(width_, height_),
    //                                         true};
    //        is_recording_ = true;
    //    }
//
    //    void StopRecording() override {
    //        std::unique_lock<std::mutex> lock(frame_mutex_);
    //        is_recording_ = false;
    //        video_writer_.release();
    //    }
//
    //    void WriteFrame(cv::Mat& frame) override {
    //        video_writer_.write(frame);
    //    }
//
    //    void AnalyzeRecording(const char* output_dir) override {
    //        std::string output_dir_str = std::string {output_dir};
    //        if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }
//
    //        std::fstream recording_info;
    //        recording_info.open(output_dir_str + "/recording_info.txt", std::ios::out);
//
    //        // Count frames
    //        cv::VideoCapture video_capture {output_dir_str + "/frames.mp4"};
    //        int frames_count = video_capture.get(cv::CAP_PROP_FRAME_COUNT);
//
    //        recording_info << "Frames: " << std::to_string(frames_count);
    //    }
//
    //private:
    //    cv::VideoWriter video_writer_;
    //};


    class PngImageEventHandler : public ImageEventHandlerBase {
    public:
        PngImageEventHandler(int height, int width, int buffer_size) : ImageEventHandlerBase(height, width), buffer_size_(buffer_size) {
            image_buffer_.reserve(buffer_size_);
        }

        void StartRecording(const char* output_dir) override {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            std::string output_dir_str {output_dir};
            if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }
            output_dir_ = output_dir;
            is_recording_ = true;
        }

        void StopRecording() override {
            // TODO: GUI becomes non-reactive while images are stored
            std::unique_lock<std::mutex> lock(frame_mutex_);
            is_recording_ = false;

            for (const auto& frame : image_buffer_) {
                std::string filename = output_dir_ + "/frame_" + std::to_string(frame_counter_) + ".png";
                processor_.Convert(frame, Spinnaker::PixelFormat_RGB8);
                frame->Save(filename.c_str());
                frame_counter_++;
            }

            image_buffer_.clear();
            frame_counter_ = 0;
        }

        void WriteFrame(Spinnaker::ImagePtr& frame) override {
            if (is_recording_ && image_buffer_.size() < buffer_size_) {
                image_buffer_.push_back(Spinnaker::Image::Create(frame));
            }
        }

        void AnalyzeRecording(const char* output_dir) override {
            std::string output_dir_str = std::string{output_dir};
            if(output_dir_str.back() == '/') {
                output_dir_str.pop_back();
            }

            std::fstream recording_info;
            recording_info.open(output_dir_str + "/recording_info.txt", std::ios::out);

            // Count PNG files in the directory
            int frames_count = 0;
            for (const auto& entry : std::filesystem::directory_iterator(output_dir_str)) {
                if (entry.path().extension() == ".png") {
                    frames_count++;
                }
            }

            recording_info << "Frames: " << std::to_string(frames_count);
            recording_info.close();
        }

    private:
        std::vector<Spinnaker::ImagePtr> image_buffer_;
        std::string output_dir_;
        int buffer_size_;
        int frame_counter_ = 0;
    };
}

#endif //RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#endif // ENABLE_SPINNAKER_SDK