#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H

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
        virtual void WriteFrame(cv::Mat& frame) = 0;

        void OnImageEvent(Spinnaker::ImagePtr image) {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            frame_ = cv::Mat(image->GetHeight() + image->GetYPadding(),
                             image->GetWidth() + image->GetXPadding(), CV_8UC1);
            frame_.data = (uchar*)image->GetData();

            if (is_recording_) {
                cv::Mat rgb_frame = cv::Mat(height_, width_, CV_8UC3);
                rgb_frame.data = frame_.data;
                cv::Mat bgr_frame;
                cv::cvtColor(rgb_frame, bgr_frame, cv::COLOR_RGB2BGR);
                this->WriteFrame(bgr_frame);
            }

            frame_received_ = true;
        }

        void OutputFrame(cv::Mat& output_frame) {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            if (frame_received_) {
                output_frame = frame_;
            } else {
                output_frame = cv::Mat::zeros(height_, width_, CV_8UC3);
            }
        }

    protected:
        cv::Mat frame_;
        std::mutex frame_mutex_;
        bool frame_received_;
        int height_, width_;
        bool is_recording_;
    };

    class Mp4ImageEventHandler : public ImageEventHandlerBase {
    public:
        Mp4ImageEventHandler(int height, int width) : ImageEventHandlerBase(height, width) {}

        void StartRecording(const char* output_dir) {
            std::unique_lock<std::mutex> lock(frame_mutex_);

            std::string output_dir_str {output_dir};
            if(output_dir_str.back() == '/') { output_dir_str.pop_back(); }

            video_writer_ = cv::VideoWriter {output_dir_str + "/frames.mp4",
                                             cv::VideoWriter::fourcc('m', 'p', '4', 'v'),
                                             30,
                                             cv::Size(width_, height_),
                                             true};
            is_recording_ = true;
        }

        void StopRecording() {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            is_recording_ = false;
            video_writer_.release();
        }

        void WriteFrame(cv::Mat& frame) {
            video_writer_.write(frame);
        }

    private:
        cv::VideoWriter video_writer_;
    };


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
            std::unique_lock<std::mutex> lock(frame_mutex_);
            is_recording_ = false;

            for (const auto& frame : image_buffer_) {
                std::string filename = output_dir_ + "/frame_" + std::to_string(frame_counter_) + ".png";
                cv::imwrite(filename, frame);
                frame_counter_++;
            }

            image_buffer_.clear();
        }

        void WriteFrame(cv::Mat& frame) override {
            if (is_recording_ && image_buffer_.size() < buffer_size_) {
                image_buffer_.push_back(frame.clone());
            }
        }

    private:
        std::vector<cv::Mat> image_buffer_;
        std::string output_dir_;
        int buffer_size_;
        int frame_counter_ = 0;
    };


}

#endif //RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#endif // ENABLE_SPINNAKER_SDK