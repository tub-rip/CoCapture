#include <boost/program_options.hpp>

#include "imgui_helper.h"
#include "recorder.h"

#ifdef ENABLE_METAVISION_SDK
#include "prophesee_wrapper.h"
#endif //ENABLE_METAVISION_SDK

#ifdef ENABLE_PYLON_SDK
#include "basler_wrapper.h"
#endif //ENABLE_PYLON_SDK

#ifdef ENABLE_SPINNAKER_SDK
#include "flir_wrapper.h"
#endif // ENABLE_SPINNAKER_SDK

using namespace rcg;

int main(int argc, char** argv) {
    // Parse program arguments
    boost::program_options::options_description options_description {"Options"};
    boost::program_options::variables_map variables_map;

    options_description.add_options()
            ("help,h", "Display help message")
            ("list,l", "List connected cameras")
            ("pick,p", boost::program_options::value<std::vector<int>>()->multitoken()->value_name("INDEX"), "Pick a camera w/ its index");

    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, options_description), variables_map);
    boost::program_options::notify(variables_map);

    if(variables_map.count("help") || variables_map.count("h") || argc == 1) {
        std::cout << options_description;
        return 0;
    }

    // Find connected cameras
    int camera_index {0};

    // Find connected Prophesee cameras
    #ifdef ENABLE_METAVISION_SDK
    std::vector<std::pair<std::string, int>> prophesee_cameras;
    for(const auto& serial_number : cams::prophesee::PropheseeCamera::ListConnectedCameras()) {
        prophesee_cameras.emplace_back(serial_number, camera_index++);
    }
    #endif //ENABLE_METAVISION_SDK

    // Find connected Basler  cameras
    #ifdef ENABLE_PYLON_SDK
    std::vector<std::pair<std::string, int>> basler_cameras;
    for(const auto& serial_number : cams::basler::BaslerCamera::ListConnectedCameras()) {
        basler_cameras.emplace_back(serial_number, camera_index++);
    }
    #endif //ENABLE_PYLON_SDK

    // Find connected Flir cameras
    #ifdef ENABLE_SPINNAKER_SDK
    std::vector<std::pair<std::string, int>> flir_cameras;
    for(const auto& serial_number : cams::flir::FlirCamera::ListConnectedCameras()) {
        flir_cameras.emplace_back(serial_number, camera_index++);
    }
    #endif // ENABLE_SPINNAKER_SDK

    // List connected cameras
    if(variables_map.count("list") || variables_map.count("l")) {
        std::cout << "Connected cameras:" << std::endl;
        #ifdef ENABLE_METAVISION_SDK
        for(const auto& prophesee_camera : prophesee_cameras) {
            std::cout << " " << " ";
            std::cout << "[" << prophesee_camera.second << "]" << " " << "prophesee" << " / " << prophesee_camera.first << std::endl;
        }
        #endif //ENABLE_METAVISION_SDK

        #ifdef ENABLE_PYLON_SDK
        for(const auto& basler_camera : basler_cameras) {
            std::cout << " " << " ";
            std::cout << "[" << basler_camera.second << "]" << " " << "basler" << " / " << basler_camera.first << std::endl;
        }
        #endif //ENABLE_PYLON_SDK

        #ifdef ENABLE_SPINNAKER_SDK
                for(const auto& flir_camera : flir_cameras) {
                    std::cout << " " << " ";
                    std::cout << "[" << flir_camera.second << "]" << " " << "flir" << " / " << flir_camera.first << std::endl;
                }
        #endif //ENABLE_PYLON_SDK
    }

    if(!variables_map.count("pick") && !variables_map.count("p")) {
        return 0;
    }

    // Initialize GUI
    gui::Initialize();

    ImGui::GetIO().IniFilename = nullptr;
    ImGui::StyleColorsLight();

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    gui::CreateWindow("rip-cocapture-gui", display_mode.w, display_mode.h);

    // Set up recorder
    tools::Recorder recorder;
    bool show_recorder = true;

    // Set up Prophesee cameras
    #ifdef ENABLE_METAVISION_SDK
    std::vector<std::pair<std::unique_ptr<wrappers::prophesee::PropheseeWrapper>, std::unique_ptr<bool>>> prophesee_wrappers;
    int prophesee_camera_index {0};
    for(const auto& picked_index : variables_map["pick"].as<std::vector<int>>()) {
        for(const auto& prophesee_camera : prophesee_cameras) {
            if(prophesee_camera.second == picked_index) {
                prophesee_wrappers.push_back({std::move(std::make_unique<wrappers::prophesee::PropheseeWrapper>(prophesee_camera.first.c_str(), prophesee_camera_index++)),
                                              std::move(std::make_unique<bool>(false))});
                recorder.RegisterWrapper(*prophesee_wrappers.back().first, *prophesee_wrappers.back().second);
            }
        }
    }
    #endif //ENABLE_METAVISION_SDK

    // Set up Basler cameras
    #ifdef ENABLE_PYLON_SDK
    std::vector<std::pair<std::unique_ptr<wrappers::basler::BaslerWrapper>, std::unique_ptr<bool>>> basler_wrappers;
    int basler_camera_index {0};
    for(const auto& picked_index : variables_map["pick"].as<std::vector<int>>()) {
        for(const auto& basler_camera : basler_cameras) {
            if(basler_camera.second == picked_index) {
                basler_wrappers.push_back({std::move(std::make_unique<wrappers::basler::BaslerWrapper>(basler_camera.first.c_str(), basler_camera_index++)),
                                           std::move(std::make_unique<bool>(false))});
                recorder.RegisterWrapper(*basler_wrappers.back().first, *basler_wrappers.back().second);
            }
        }
    }
    # endif // ENABLE_PYLON_SDK

    # ifdef ENABLE_SPINNAKER_SDK
    //std::vector<std::pair<std::unique_ptr<wrappers::flir::FlirWrapper>, std::unique_ptr<bool>>> flir_wrappers;
    //int flir_camera_index {0};
    //for(const auto& picked_index : variables_map["pick"].as<std::vector<int>>()) {
    //    for(const auto& flir_camera : flir_cameras) {
    //        if(basler_camera.second == picked_index) {
    //            basler_wrappers.push_back({std::move(std::make_unique<wrappers::basler::BaslerWrapper>(basler_camera.first.c_str(), basler_camera_index++)),
    //                                       std::move(std::make_unique<bool>(false))});
    //            recorder.RegisterWrapper(*basler_wrappers.back().first, *basler_wrappers.back().second);
    //        }
    //    }
    //}
    #endif

    // Program loop
    bool done = false;
    while(!done) {
        gui::HandleEvent(done);
        gui::NewFrame();

        // Display recorder
        recorder.Show(&show_recorder);

        gui::EndFrame();
    }

    gui::DestroyWindow();
    gui::Terminate();
}