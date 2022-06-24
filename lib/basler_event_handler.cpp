#include "basler_event_handler.h"

namespace basler {
    void BaslerEventHandler::get_display_frame(cv::Mat &display) {
        std::unique_lock<std::mutex> lock(m_);
        img_.copyTo(display);
    }


    void BaslerEventHandler::OnImageGrabbed(Pylon::CInstantCamera &camera,
                                            const Pylon::CGrabResultPtr &grab_result) {
        // TODO: include if grab succeeded
        std::unique_lock<std::mutex> lock(m_);
        cv::Mat frame(grab_result->GetHeight(), grab_result->GetWidth(),
                      CV_8UC1, (uint8_t*)grab_result->GetBuffer());
        cv::cvtColor(frame, img_, cv::COLOR_GRAY2RGB);
    }

} // basler