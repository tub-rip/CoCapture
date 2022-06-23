#include <iostream>
#include <thread>

#include <opencv2/opencv.hpp>
#include <metavision/sdk/driver/camera.h>
#include <metavision/sdk/base/utils/log.h>

#include "../lib/event_queue.h"

/*
 * Current Implementation seems super slow
 * Probably setting a mutex for every event pop() call is not
 * feasible
 * rather make lock to read all events until the next frame is released
 *
 * queue_events(begin, end) -> writes events to queue
 * make_frame(next_gray_frame time_stamp, next_time_stamp)
 * -> pops all events from queue before frame
 * -> writes all events in frame until next frame
 *
 * display_frame ->
 *
 */

int main() {
    std::cout << "Starting co-capture visualization!" << std::endl;

    event_queue::EventQueue event_queue;

    Metavision::Camera event_camera = Metavision::Camera::from_first_available();
    event_camera.add_runtime_error_callback([](const Metavision::CameraException &e) {
        MV_LOG_ERROR() << e.what();
        throw;
    });

    event_camera.cd().add_callback(
            [&event_queue](const Metavision::EventCD *begin, const Metavision::EventCD *end) {
                event_queue.queue_events(begin, end);
            });

    event_camera.start();


    const Metavision::Geometry & geometry = event_camera.geometry();
    cv::namedWindow("Visualization", cv::WINDOW_GUI_EXPANDED);
    cv::resizeWindow("Visualization", geometry.width(), geometry.height());
    bool stop_application = false;

    cv::Vec3b color_bg = cv::Vec3b(52, 37, 30);
    cv::Vec3b color_on = cv::Vec3b(236, 223, 216);
    cv::Vec3b color_off = cv::Vec3b(201, 126, 64);

    cv::Mat display(geometry.height(), geometry.width(), CV_8UC3);
    display.setTo(color_bg);
    cv::imshow("Visualization", display);
    cv::waitKey(1);

    long next_time_step = 33000;
    int time_delta = 33000;

    while(!stop_application) {

        std::optional<Metavision::EventCD> ev = event_queue.pop();
        if (ev) {
            if (ev->t > next_time_step) {
                cv::imshow("Visualization", display);
                display.setTo(color_bg);
                int key = cv::waitKey(1);
                if ((key & 0xff) == 'q') {
                    stop_application = true;
                    std::cout << "q pressed, exiting." << std::endl;
                }
            }
            display.at<cv::Vec3b>(ev->y, ev->x) = (ev->p) ? color_on : color_off;
        }
    }
}