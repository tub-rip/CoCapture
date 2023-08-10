#ifdef ENABLE_REALSENSE_SDK
#ifndef RIP_COCAPTURE_GUI_REALSENSE_CAMERA_H
#define RIP_COCAPTURE_GUI_REALSENSE_CAMERA_H

#include <librealsense2/rs.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>

namespace rcg::cams::realsense {

    enum ProductLine {
        PROD_LINE_L5
    };

    class RealSenseCamera {
    public:
        RealSenseCamera();
        virtual ~RealSenseCamera();

    public:
        virtual rs2::device& GetDevice() = 0;
        virtual void Stream() = 0;
        virtual bool IsStarted() = 0;
        virtual bool Start() = 0;
        virtual bool Stop() = 0;
        virtual void OutputFrame(cv::Mat& frame) = 0;
        virtual std::pair<int, int> GetOutputFrameDimensions() = 0;
        virtual bool IsRecording() = 0;
        virtual bool StartRecording(const char* output_dir) = 0;
        virtual bool StopRecording() = 0;

    public:
        static rs2::context& GetContext();
        static std::vector<std::string> ListConnectedCameras();
        static ProductLine GetProductLine(rs2::device& device);
        static void MetadataToCSV(rs2::frame& frame, std::string file_name);
    };

} // rcg::cams::realsense

#endif //RIP_COCAPTURE_GUI_REALSENSE_CAMERA_H
#endif //ENABLE_REALSENSE_SDK
