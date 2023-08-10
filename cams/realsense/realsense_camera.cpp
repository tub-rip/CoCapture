#ifdef ENABLE_REALSENSE_SDK

#include "realsense_camera.h"

namespace rcg::cams::realsense {

    RealSenseCamera::RealSenseCamera() {}

    RealSenseCamera::~RealSenseCamera() {}

    rs2::context& RealSenseCamera::GetContext() {
        static rs2::context ctx;
        return ctx;
    }

    std::vector<std::string> RealSenseCamera::ListConnectedCameras() {
        std::vector<std::string> serial_numbers;

        rs2::device_list devices = GetContext().query_devices();
        for(rs2::device device : devices) {
            serial_numbers.push_back(device.get_info(RS2_CAMERA_INFO_SERIAL_NUMBER));
        }

        return serial_numbers;
    }

    ProductLine RealSenseCamera::GetProductLine(rs2::device& device) {
        std::string product_line = device.get_info(RS2_CAMERA_INFO_PRODUCT_LINE);

        if(product_line.find("L5") != std::string::npos) {
            return PROD_LINE_L5;
        }

        return {};
    }

    void RealSenseCamera::MetadataToCSV(rs2::frame& frame, std::string file_name) {
        std::fstream csv;
        csv.open(file_name, std::ios::out);

        csv << "Stream," << rs2_stream_to_string(frame.get_profile().stream_type()) << "\nMetadata Attribute,Value\n";

        // Record all available metadata attributes
        for(size_t i = 0; i < RS2_FRAME_METADATA_COUNT; ++i) {
            if(frame.supports_frame_metadata((rs2_frame_metadata_value) i)) {
                csv << rs2_frame_metadata_to_string((rs2_frame_metadata_value) i) << ","
                    << frame.get_frame_metadata((rs2_frame_metadata_value) i) << "\n";
            }
        }

        csv.close();
    }

} // rcg::cams::realsense

#endif //ENABLE_REALSENSE_SDK