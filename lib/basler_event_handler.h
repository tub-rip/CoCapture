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
    BaslerEventHandler(bool do_warp, Parameters params) :
        do_warp_(do_warp),
        params_(params)
        {
        cv::namedWindow("Debug", cv::WINDOW_KEEPRATIO);
        }
    ~BaslerEventHandler() override = default;

    void get_display_frame(cv::Mat & display) {
        {
            std::unique_lock<std::mutex> lock(m_);
            std::swap(img_, img_swap_);
        }


        // DEBUG
        cv::Mat homography(3, 3, CV_64F);
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                homography.at<double>(i, j) = params_.homography(i, j);
                //std::cout << homography.at<double>(i, j);
            }
            //std::cout << std::endl;
        }


        // END DEBUG

        cv::Mat tmp(params_.target_width, params_.target_height, CV_8UC1);
        if (params_.do_warp) {
            cv::warpPerspective(img_swap_, tmp, homography,
                                cv::Size(params_.target_width,
                                         params_.target_height));
            cv::imshow("Debug", tmp);
            cv::waitKey(1);
        } else {
            img_swap_.copyTo(tmp);
        }
        cv::flip(tmp, tmp, 0);
        cv::cvtColor(tmp, display, cv::COLOR_GRAY2RGB);
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
    Parameters params_;
};

} // basler

#endif //RIP_COCAPTURE_GUI_BASLER_EVENT_HANDLER_H
