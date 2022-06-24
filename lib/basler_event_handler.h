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
    void get_display_frame(cv::Mat & display);
    virtual void OnImageGrabbed(Pylon::CInstantCamera& camera,
                                const Pylon::CGrabResultPtr& grab_result);
    // TODO: include OnImageSkipped

private:
    cv::Mat img_;
    std::mutex m_;
};

} // basler

#endif //RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H
