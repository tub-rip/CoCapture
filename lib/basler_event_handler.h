#ifndef RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H
#define RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <pylon/PylonIncludes.h>
#include <pylon/BaslerUniversalInstantCamera.h>

#include "parameters.h"


namespace basler {

class BaslerEventHandler : public Pylon::CImageEventHandler {
public:
    BaslerEventHandler(BaslerParams params) :
        do_warp_(params.do_warp),
        params_(params) {}
    ~BaslerEventHandler() override = default;

    void get_display_frame(cv::Mat & display) {
        {
            std::unique_lock<std::mutex> lock(m_);
            std::swap(img_, img_swap_);
        }

        if (do_warp_) {
            cv::warpPerspective(img_swap_, img_swap_, params_.homography,
                                cv::Size(params_.target_width,
                                         params_.target_height));
        }
        cv::cvtColor(img_swap_, display, cv::COLOR_GRAY2RGB);
    }

    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera,
                                const Pylon::CGrabResultPtr& grab_result) {
        if (grab_result->GrabSucceeded()) {
            std::unique_lock<std::mutex> lock(m_);
            cv::Mat frame(grab_result->GetHeight(), grab_result->GetWidth(),
                          CV_8UC1, (uint8_t*)grab_result->GetBuffer());
            frame.copyTo(img_);
        }
        else {
            std::cout << "Error: " << std::hex << grab_result->GetErrorCode() <<
                      std::dec << " " << grab_result->GetErrorDescription() << std::endl;
        }
    }
    // TODO: include OnImageSkipped

private:
    cv::Mat img_;
    cv::Mat img_swap_;
    std::mutex m_;
    bool do_warp_;
    BaslerParams params_;
};

} // basler

#endif //RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H
