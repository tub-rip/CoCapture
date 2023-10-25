#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H

#include <filesystem>
#include <future>

#include <boost/filesystem.hpp>


namespace rcg::cams::flir {
    class ImageEventHandlerBase : public Spinnaker::ImageEventHandler {
    public:
        ImageEventHandlerBase(int height, int width) :
                frame_received_(false),
                height_(height),
                width_(width),
                is_recording_(false) {
            processor_.SetColorProcessing(Spinnaker::SPINNAKER_COLOR_PROCESSING_ALGORITHM_HQ_LINEAR);
        }

        virtual void StartRecording(const char* output_dir) = 0;
        virtual void StopRecording() = 0;
        virtual void WriteFrame(Spinnaker::ImagePtr& frame) = 0;
        virtual void AnalyzeRecording(const char* output_dir, int exposure_time) = 0;

        void OnImageEvent(Spinnaker::ImagePtr image) override {
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

            std::string img_dir = output_dir_ + "/imgs";
            boost::filesystem::create_directory(img_dir);

            #pragma omp parallel for default(none) shared(image_buffer_, output_dir_, processor_)
            for (int frame_id=0; frame_id < image_buffer_.size(); ++frame_id) {
                auto& frame = image_buffer_[frame_id];
                std::string frame_id_name = std::to_string(frame_id);
                frame_id_name.insert(frame_id_name.begin(), 6 - frame_id_name.size(),'0');
                std::string filename = output_dir_ + "/imgs/frame_" + frame_id_name + ".png";
                processor_.Convert(frame, Spinnaker::PixelFormat_RGB8);
                Spinnaker::ImagePtr converted = processor_.Convert(frame, Spinnaker::PixelFormat_RGB8);
                converted->Save(filename.c_str());
            }

            image_buffer_.clear();
        }

        void WriteFrame(Spinnaker::ImagePtr& frame) override {
            if (is_recording_) {
                image_buffer_.push_back(Spinnaker::Image::Create(frame));
            }
        }

        void AnalyzeRecording(const char* output_dir, int exposure_time) override {
            std::string output_dir_str = std::string{output_dir};
            if(output_dir_str.back() == '/') {
                output_dir_str.pop_back();
            }

            std::fstream recording_info;
            recording_info.open(output_dir_str + "/recording_info.yaml", std::ios::out);

            // Count PNG files in the directory
            int frames_count = 0;
            for (const auto& entry : std::filesystem::directory_iterator(output_dir_str + "/imgs")) {
                if (entry.path().extension() == ".png") {
                    frames_count++;
                }
            }

            recording_info << "num_frames: " << std::to_string(frames_count) << std::endl;
            recording_info << "exposure_time: " << exposure_time << std::endl;

            recording_info.close();
        }

    private:
        std::vector<Spinnaker::ImagePtr> image_buffer_;
        std::string output_dir_;
        int buffer_size_;
    };
}

#endif //RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#endif // ENABLE_SPINNAKER_SDK