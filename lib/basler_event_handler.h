#ifndef RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H

#include <opencv2/opencv.hpp>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>


namespace basler {

class BaslerEventHandler : public Pylon::CImageEventHandler {
public:
    BaslerEventHandler() = default;
    ~BaslerEventHandler() override = default;

    void get_display_frame(cv::Mat & display) {
        std::unique_lock<std::mutex> lock(m_);
        img_.copyTo(display);
    }

    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera,
                                const Pylon::CGrabResultPtr& grab_result) {
        if (grab_result->GrabSucceeded()) {
            std::unique_lock<std::mutex> lock(m_);
            cv::Mat frame(grab_result->GetHeight(), grab_result->GetWidth(),
                          CV_8UC1, (uint8_t*)grab_result->GetBuffer());
            cv::cvtColor(frame, img_, cv::COLOR_GRAY2RGB);
        }
        else {
            std::cout << "Error: " << std::hex << grab_result->GetErrorCode() <<
                      std::dec << " " << grab_result->GetErrorDescription() << std::endl;
        }
    }
    // TODO: include OnImageSkipped

private:
    cv::Mat img_;
    std::mutex m_;
};

} // basler

#endif //RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H
