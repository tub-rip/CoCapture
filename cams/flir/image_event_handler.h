#ifdef ENABLE_SPINNAKER_SDK
#ifndef RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H

namespace rcg::cams::flir {
    class ImageEventHandler : public Spinnaker::ImageEventHandler {
    public:
        ImageEventHandler(int height, int width) :
            frame_received_(false),
            height_(height),
            width_(width) {}

        void OnImageEvent(Spinnaker::ImagePtr image) {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            frame_ = image;
            frame_received_ = true;
        }

        void OutputFrame(cv::Mat& output_frame) {
            std::unique_lock<std::mutex> lock(frame_mutex_);
            if (frame_received_) {
                //std::cout << frame_->GetPixelFormat() << std::endl;
                //cv::Mat

                output_frame = cv::Mat(frame_->GetHeight() + frame_->GetYPadding(),
                                frame_->GetWidth() + frame_->GetXPadding(), CV_8UC3);
                output_frame.data = (uchar*)frame_->GetData();
                //cv::cvtColor(bayer_frame, output_frame, cv::COLOR_BayerRG2BGR);
            } else {
                output_frame = cv::Mat::zeros(height_, width_, CV_8UC3);
            }
            //frame_received_ = false;
        }
    private:
        Spinnaker::ImagePtr frame_;
        std::mutex frame_mutex_;
        bool frame_received_;
        int height_, width_;
    };
}

#endif //RIP_COCAPTURE_GUI_FLIR_IMAGE_EVENT_HANDLER_H
#endif // ENABLE_SPINNAKER_SDK