#ifdef ENABLE_DEAR_IMGUI

#include <unistd.h>
#include <ctime>

#include <boost/filesystem.hpp>

#include "gui.h"
#include "launcher_interface.h"
#include "camera_interface.h"

using namespace Cocapture;

const char* kApplicationName = "Cocapture";
const char* kApplicationVersion = "1.0";

// Gui
Window* win = nullptr;
ImVec4 clear_color = {0.45f, 0.55f, 0.60f, 1.0f};

// Allocated textures
std::vector<GLuint*> textures;

// Launchers
std::vector<std::unique_ptr<ILauncher>> camera_launchers;

// Camera models
std::vector<std::unique_ptr<IModel>> camera_models;

// Camera identifiers
std::vector<std::string> camera_identifiers;

// Cameras
std::vector<std::unique_ptr<ICamera>> cameras;

// Recordings
std::map<std::string, std::vector<int>> recordings;

// Basler cameras
#ifdef ENABLE_PYLON_SDK
#include "basler_launcher.h"
#include "basler_camera.h"
#endif //ENABLE_PYLON_SDK

int n_basler_cameras;
bool* basler_cameras_selected;
int* basler_trigger_selector_selected;
int* basler_trigger_source_selected;
int* basler_trigger_activation_selected;

// Prophesee cameras
#ifdef ENABLE_METAVISION_SDK
#include "prophesee_launcher.h"
#include "basler_camera.h"
#endif //ENABLE_METAVISION_SDK

int n_prophesee_cameras;
bool* prophesee_cameras_selected;
int* prophesee_sync_mode_selected;

bool CocaptureLauncher() {
    // Resource allocation
    for(const auto& camera_launcher : camera_launchers) {
        if(camera_launcher->Type == CAM_TYPE_BASLER) {
            n_basler_cameras = camera_launcher->GetCameraIdentifiers().size();
            basler_cameras_selected = new bool[n_basler_cameras]();
            basler_trigger_selector_selected = new int[n_basler_cameras]();
            basler_trigger_source_selected = new int[n_basler_cameras]();
            basler_trigger_activation_selected = new int[n_basler_cameras]();
        }

        if(camera_launcher->Type == CAM_TYPE_PROPHESEE) {
            n_prophesee_cameras = camera_launcher->GetCameraIdentifiers().size();
            prophesee_cameras_selected = new bool[n_prophesee_cameras]();
            prophesee_sync_mode_selected = new int[n_prophesee_cameras]();
        }
    }

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    win = CreateWindow(kApplicationName, 0.275f * display_mode.w, 0.45f * display_mode.h);

    bool done = false;
    bool start = false;
    while(!done) {
        HandleEvent(win, done);
        NewFrame(win);

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGuiWindowFlags window_flags = (ImGuiWindowFlags) ImGuiWindowFlags_MenuBar |
                                                           ImGuiWindowFlags_NoTitleBar |
                                                           ImGuiWindowFlags_NoCollapse |
                                                           ImGuiWindowFlags_NoResize |
                                                           ImGuiWindowFlags_NoMove;
        ImGui::Begin("##CocaptureLauncher", nullptr, window_flags);
        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("Menu")) {
                char* args[] = {nullptr};
                if(ImGui::MenuItem("Restart")) { execvp("./cocapture", args); }
                if(ImGui::MenuItem("Quit", "Alt + F4")) { done = true; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        // Camera configuration
        for(const auto& camera_launcher : camera_launchers) {
            if(camera_launcher->GetCameraIdentifiers().empty()) { continue; }
            if(camera_launcher->Type == CAM_TYPE_BASLER) {
                if(ImGui::CollapsingHeader("Basler")) {
                    int basler_camera_index = 0;
                    for(const auto& basler_camera_identifier : camera_launcher->GetCameraIdentifiers()) {
                        ImGui::SeparatorText(("Device #" + std::to_string(basler_camera_index + 1)).c_str());
                        ImGui::Spacing();

                        ImGui::Text("Serial number      : %s", camera_launcher->GetCameraInformation(basler_camera_identifier, BASLER_INFO_SERIAL).c_str());
                        ImGui::Spacing();

                        ImGui::Text("Model name         : %s", camera_launcher->GetCameraInformation(basler_camera_identifier, BASLER_INFO_MODEL).c_str());
                        ImGui::Spacing();

                        ImGui::Text("Vendor name        : %s", camera_launcher->GetCameraInformation(basler_camera_identifier, BASLER_INFO_VENDOR).c_str());
                        ImGui::Spacing();

                        std::vector<std::string> basler_trigger_selector_options_vec = camera_launcher->GetCameraLaunchOptions(basler_camera_identifier, BASLER_TRIGGER_SELECTOR);
                        basler_trigger_selector_options_vec.insert(basler_trigger_selector_options_vec.begin(), "Select trigger type");
                        const char* basler_trigger_selector_options_arr[basler_trigger_selector_options_vec.size()];
                        for(int i = 0; i < basler_trigger_selector_options_vec.size(); ++i) {
                            basler_trigger_selector_options_arr[i] = basler_trigger_selector_options_vec[i].c_str();
                        }
                        ImGui::Text("Trigger selector  ");
                        ImGui::SameLine();
                        ImGui::Combo(("##BaslerTriggerSelector" + std::to_string(basler_camera_index)).c_str(),
                                      &basler_trigger_selector_selected[basler_camera_index],
                                      basler_trigger_selector_options_arr,
                                      IM_ARRAYSIZE(basler_trigger_selector_options_arr));
                        ImGui::Spacing();

                        std::vector<std::string> basler_trigger_source_options_vec = camera_launcher->GetCameraLaunchOptions(basler_camera_identifier, BASLER_TRIGGER_SOURCE);
                        basler_trigger_source_options_vec.insert(basler_trigger_source_options_vec.begin(), "Select trigger source");
                        const char* basler_trigger_source_options_arr[basler_trigger_source_options_vec.size()];
                        for(int i = 0; i < basler_trigger_source_options_vec.size(); ++i) {
                            basler_trigger_source_options_arr[i] = basler_trigger_source_options_vec[i].c_str();
                        }
                        ImGui::Text("Trigger source    ");
                        ImGui::SameLine();
                        ImGui::Combo(("##BaslerTriggerSource" + std::to_string(basler_camera_index)).c_str(),
                                     &basler_trigger_source_selected[basler_camera_index],
                                     basler_trigger_source_options_arr,
                                     IM_ARRAYSIZE(basler_trigger_source_options_arr));
                        ImGui::Spacing();

                        std::vector<std::string> basler_trigger_activation_options_vec = camera_launcher->GetCameraLaunchOptions(basler_camera_identifier, BASLER_TRIGGER_ACTIVATION);
                        basler_trigger_activation_options_vec.insert(basler_trigger_activation_options_vec.begin(), "Select trigger activation");
                        const char* basler_trigger_activation_options_arr[basler_trigger_activation_options_vec.size()];
                        for(int i = 0; i < basler_trigger_activation_options_vec.size(); ++i) {
                            basler_trigger_activation_options_arr[i] = basler_trigger_activation_options_vec[i].c_str();
                        }
                        ImGui::Text("Trigger activation");
                        ImGui::SameLine();
                        ImGui::Combo(("##BaslerTriggerActivation" + std::to_string(basler_camera_index)).c_str(),
                                     &basler_trigger_activation_selected[basler_camera_index],
                                     basler_trigger_activation_options_arr,
                                     IM_ARRAYSIZE(basler_trigger_activation_options_arr));
                        ImGui::Spacing();

                        ImGui::Text("Launch            ");
                        ImGui::SameLine();
                        ImGui::Checkbox(("##BaslerLaunch" + std::to_string(basler_camera_index)).c_str(), &basler_cameras_selected[basler_camera_index]);
                        ImGui::Spacing();

                        ++basler_camera_index;
                    }
                }
            }

            if(camera_launcher->Type == CAM_TYPE_PROPHESEE) {
                if(ImGui::CollapsingHeader("Prophesee")) {
                    int prophesee_camera_index = 0;
                    for(const auto& prophesee_camera_identifier : camera_launcher->GetCameraIdentifiers()) {
                        ImGui::SeparatorText(("Device #" + std::to_string(prophesee_camera_index + 1)).c_str());
                        ImGui::Spacing();

                        ImGui::Text("Serial number        : %s", camera_launcher->GetCameraInformation(prophesee_camera_identifier, PROPHESEE_INFO_SERIAL).c_str());
                        ImGui::Spacing();

                        ImGui::Text("Integrator name      : %s", camera_launcher->GetCameraInformation(prophesee_camera_identifier, PROPHESEE_INFO_INTEGRATOR).c_str());
                        ImGui::Spacing();

                        ImGui::Text("Plugin name          : %s", camera_launcher->GetCameraInformation(prophesee_camera_identifier, PROPHESEE_INFO_PLUGIN).c_str());
                        ImGui::Spacing();

                        std::vector<std::string> prophesee_sync_mode_options_vec = camera_launcher->GetCameraLaunchOptions(prophesee_camera_identifier, PROPHESEE_SYNC_MODE);
                        prophesee_sync_mode_options_vec.insert(prophesee_sync_mode_options_vec.begin(), "Select synchronization mode");
                        const char* prophesee_sync_mode_options_arr[prophesee_sync_mode_options_vec.size()];
                        for(int i = 0; i < prophesee_sync_mode_options_vec.size(); ++i) {
                            prophesee_sync_mode_options_arr[i] = prophesee_sync_mode_options_vec[i].c_str();
                        }
                        ImGui::Text("Synchronization mode");
                        ImGui::SameLine();
                        ImGui::Combo(("##PropheseeSynchronizationMode" + std::to_string(prophesee_camera_index)).c_str(),
                                     &prophesee_sync_mode_selected[prophesee_camera_index],
                                     prophesee_sync_mode_options_arr,
                                     IM_ARRAYSIZE(prophesee_sync_mode_options_arr));
                        ImGui::Spacing();

                        ImGui::Text("Launch              ");
                        ImGui::SameLine();
                        ImGui::Checkbox(("##PropheseeLaunch" + std::to_string(prophesee_camera_index)).c_str(), &prophesee_cameras_selected[prophesee_camera_index]);
                        ImGui::Spacing();

                        ++prophesee_camera_index;
                    }
                }
            }
        }

        float next_cursor_pos_y = std::max(ImGui::GetWindowContentRegionMax().y * (1 - 0.04f), ImGui::GetCursorPos().y);
        ImGui::SetCursorPosY(next_cursor_pos_y);

        // Camera startup
        bool camera_selected = false;
        for(const auto& camera_launcher : camera_launchers) {
            if(camera_launcher->Type == CAM_TYPE_BASLER) {
                for(int i = 0; i < n_basler_cameras; ++i) { if(basler_cameras_selected[i]) { camera_selected = true; break; } }
            }

            if(camera_launcher->Type == CAM_TYPE_PROPHESEE) {
                for(int i = 0; i < n_prophesee_cameras; ++i) { if(prophesee_cameras_selected[i]) { camera_selected = true; break; } }
            }
        }

        ImGui::BeginDisabled(!camera_selected);
        if(ImGui::Button("Start")) {
            for(const auto& camera_launcher : camera_launchers) {
                if(camera_launcher->Type == CAM_TYPE_BASLER) {
                    int basler_camera_index = 0;
                    for(const auto& basler_camera_identifier : camera_launcher->GetCameraIdentifiers()) {
                        if(basler_cameras_selected[basler_camera_index]) {
                            if(basler_trigger_selector_selected[basler_camera_index]) {
                                camera_launcher->SetCameraLaunchOption(basler_camera_identifier,
                                                                       BASLER_TRIGGER_SELECTOR,
                                                                       camera_launcher->GetCameraLaunchOptions(
                                                                               basler_camera_identifier,
                                                                               BASLER_TRIGGER_SELECTOR)[--basler_trigger_selector_selected[basler_camera_index]]);
                            }

                            if(basler_trigger_source_selected[basler_camera_index]) {
                                camera_launcher->SetCameraLaunchOption(basler_camera_identifier,
                                                                       BASLER_TRIGGER_SOURCE,
                                                                       camera_launcher->GetCameraLaunchOptions(
                                                                               basler_camera_identifier,
                                                                               BASLER_TRIGGER_SOURCE)[--basler_trigger_source_selected[basler_camera_index]]);
                            }

                            if(basler_trigger_activation_selected[basler_camera_index]) {
                                camera_launcher->SetCameraLaunchOption(basler_camera_identifier,
                                                                       BASLER_TRIGGER_ACTIVATION,
                                                                       camera_launcher->GetCameraLaunchOptions(
                                                                               basler_camera_identifier,
                                                                               BASLER_TRIGGER_ACTIVATION)[--basler_trigger_activation_selected[basler_camera_index]]);
                            }

                            std::unique_ptr<ICamera> basler_camera = camera_launcher->GetCamera(basler_camera_identifier);
                            std::unique_ptr<IModel> basler_model = camera_launcher->GetModel(basler_camera, basler_camera_index);

                            GLuint* basler_texture = new GLuint[1];
                            textures.push_back(basler_texture);
                            GenerateTexture(basler_texture,
                                            basler_camera->GetProp(BASLER_DIM_WIDTH),
                                            basler_camera->GetProp(BASLER_DIM_HEIGHT));

                            basler_model->SetRef(BASLER_GUI_TEXTURE, basler_texture);
                            basler_camera->Start();
                            camera_identifiers.push_back("[" + std::string((char*) basler_model->GetRef(BASLER_GUI_TITLE)) + "] " +
                                                         basler_camera->GetInfo(BASLER_INFO_SERIAL));
                            camera_models.push_back(std::move(basler_model));
                            cameras.push_back(std::move(basler_camera));
                        }

                        ++basler_camera_index;
                    }
                }

                if(camera_launcher->Type == CAM_TYPE_PROPHESEE) {
                    int prophesee_camera_index = 0;
                    for(const auto& prophesee_camera_identifier : camera_launcher->GetCameraIdentifiers()) {
                        if(prophesee_cameras_selected[prophesee_camera_index]) {
                            if(prophesee_sync_mode_selected[prophesee_camera_index]) {
                                camera_launcher->SetCameraLaunchOption(prophesee_camera_identifier,
                                                                       PROPHESEE_SYNC_MODE,
                                                                       camera_launcher->GetCameraLaunchOptions(
                                                                               prophesee_camera_identifier,
                                                                               PROPHESEE_SYNC_MODE)[--prophesee_sync_mode_selected[prophesee_camera_index]]);
                            }

                            std::unique_ptr<ICamera> prophesee_camera = camera_launcher->GetCamera(prophesee_camera_identifier);
                            std::unique_ptr<IModel> prophesee_model = camera_launcher->GetModel(prophesee_camera, prophesee_camera_index);

                            GLuint* prophesee_texture = new GLuint[1];
                            textures.push_back(prophesee_texture);
                            GenerateTexture(prophesee_texture,
                                            prophesee_camera->GetProp(PROPHESEE_GEOMETRY_WIDTH),
                                            prophesee_camera->GetProp(PROPHESEE_GEOMETRY_HEIGHT));

                            prophesee_model->SetRef(PROPHESEE_GUI_TEXTURE, prophesee_texture);
                            prophesee_camera->Start();
                            camera_identifiers.push_back("[" + std::string((char*) prophesee_model->GetRef(PROPHESEE_GUI_TITLE)) + "] " +
                                                         prophesee_camera->GetInfo(PROPHESEE_INFO_SERIAL));
                            camera_models.push_back(std::move(prophesee_model));
                            cameras.push_back(std::move(prophesee_camera));
                        }

                        ++prophesee_camera_index;
                    }
                }
            }

            if(!cameras.empty()) {
                done = true;
                start = true;
            }
        }
        ImGui::EndDisabled();

        ImGui::SameLine();

        // Refresh launcher
        if(ImGui::Button("Refresh")) {
            for(const auto& camera_launcher : camera_launchers) {
                camera_launcher->Reset();

                if(camera_launcher->Type == CAM_TYPE_BASLER) {
                    n_basler_cameras = camera_launcher->GetCameraIdentifiers().size();

                    delete basler_cameras_selected;
                    basler_cameras_selected = new bool[n_basler_cameras]();

                    delete basler_trigger_selector_selected;
                    basler_trigger_selector_selected = new int[n_basler_cameras]();

                    delete basler_trigger_source_selected;
                    basler_trigger_source_selected = new int[n_basler_cameras]();

                    delete basler_trigger_activation_selected;
                    basler_trigger_activation_selected = new int[n_basler_cameras]();
                }

                if(camera_launcher->Type == CAM_TYPE_PROPHESEE) {
                    n_prophesee_cameras = camera_launcher->GetCameraIdentifiers().size();

                    delete prophesee_cameras_selected;
                    prophesee_cameras_selected = new bool[n_prophesee_cameras]();

                    delete prophesee_sync_mode_selected;
                    prophesee_sync_mode_selected = new int[n_prophesee_cameras]();
                }
            }
        }

        ImGui::End();
        EndFrame(win, clear_color);
    }

    // Free up resources
    for(const auto& camera_launcher : camera_launchers) {
        if(camera_launcher->Type == CAM_TYPE_BASLER) {
            delete basler_cameras_selected;
            delete basler_trigger_selector_selected;
            delete basler_trigger_source_selected;
            delete basler_trigger_activation_selected;
        }

        if(camera_launcher->Type == CAM_TYPE_PROPHESEE) {
            delete prophesee_cameras_selected;
            delete prophesee_sync_mode_selected;
        }
    }

    return start;
}

void CocaptureApplication() {
    int cameras_available_selected = 0;
    bool cameras_available_show[cameras.size()];
    bool cameras_available_recording[cameras.size()];
    const char* cameras_available_options[cameras.size()];

    bool recording_window_show = false;
    char custom_recording_identifier[128];
    memset(custom_recording_identifier, 0, sizeof(custom_recording_identifier));

    bool multi_camera_recording = false;
    int recording_selected = 0;
    std::pair<std::string, std::vector<int>> current_recording;

    for(int i = 0; i < cameras.size(); ++i) {
        cameras_available_show[i] = false;
        cameras_available_recording[i] = false;
        cameras_available_options[i] = camera_identifiers[i].c_str();
    }

    cameras_available_show[0] = true;

    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);

    ResizeWindow(win, display_mode.w, display_mode.h);

    bool done = false;
    while(!done) {
        HandleEvent(win, done);
        NewFrame(win);

        ImGui::SetNextWindowPos(ImGui::GetMainViewport()->Pos);
        ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
        ImGuiWindowFlags window_flags = (ImGuiWindowFlags) ImGuiWindowFlags_NoBringToFrontOnFocus |
                                        ImGuiWindowFlags_MenuBar |
                                        ImGuiWindowFlags_NoTitleBar |
                                        ImGuiWindowFlags_NoCollapse |
                                        ImGuiWindowFlags_NoResize |
                                        ImGuiWindowFlags_NoMove;
        ImGui::Begin("##CocaptureApplication", nullptr, window_flags);
        if(ImGui::BeginMenuBar()) {
            if(ImGui::BeginMenu("Menu")) {
                char* args[] = {nullptr};
                if(ImGui::MenuItem("Restart")) {
                    for(const auto& camera : cameras) { camera->Clean(); }
                    execvp("./cocapture", args);
                }
                if(ImGui::MenuItem("Quit", "Alt + F4")) { done = true; }
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        ImVec2 menu_bar_size = ImGui::GetItemRectSize();

        // Update camera textures
        for(int i = 0; i < cameras.size(); ++i) {
            if(camera_models[i]->Type == CAM_TYPE_BASLER) {
                cv::Mat image_frame;
                cameras[i]->OutputFrame({}, image_frame);

                GLuint* basler_texture = (GLuint*) camera_models[i]->GetRef(BASLER_GUI_TEXTURE);
                UpdateTexture(basler_texture, image_frame.data,
                              cameras[i]->GetProp(BASLER_DIM_WIDTH), cameras[i]->GetProp(BASLER_DIM_HEIGHT));
            }

            if(camera_models[i]->Type == CAM_TYPE_PROPHESEE) {
                cv::Mat cd_frame;
                cameras[i]->OutputFrame({}, cd_frame);

                GLuint* prophesee_texture = (GLuint*) camera_models[i]->GetRef(PROPHESEE_GUI_TEXTURE);
                UpdateTexture(prophesee_texture, cd_frame.data,
                              cameras[i]->GetProp(PROPHESEE_GEOMETRY_WIDTH), cameras[i]->GetProp(PROPHESEE_GEOMETRY_HEIGHT));
            }
        }

        ImVec2 camera_viz_space = {0.76f * ImGui::GetWindowSize().x, ImGui::GetWindowSize().y - menu_bar_size.y};
        ImVec2 camera_viz_pos = {ImGui::GetWindowPos().x + camera_viz_space.x / 2,
                                 ImGui::GetWindowPos().y + (ImGui::GetWindowSize().y + menu_bar_size.y) / 2};

        // Draw camera visualization
        for(int i = 0; i < cameras.size(); ++i) {
            if(camera_models[i]->Type == CAM_TYPE_BASLER) {
                GLuint* basler_texture = (GLuint*) camera_models[i]->GetRef(BASLER_GUI_TEXTURE);

                int width = cameras[i]->GetProp(BASLER_DIM_WIDTH);
                int height = cameras[i]->GetProp(BASLER_DIM_HEIGHT);

                ShowViz(basler_texture, camera_viz_pos, ResizeViz(width, height, camera_viz_space),
                       (char*) camera_models[i]->GetRef(BASLER_GUI_TITLE), cameras_available_show[i]);
            }

            if(camera_models[i]->Type == CAM_TYPE_PROPHESEE) {
                GLuint* prophesee_texture = (GLuint*) camera_models[i]->GetRef(PROPHESEE_GUI_TEXTURE);

                int width = cameras[i]->GetProp(PROPHESEE_GEOMETRY_WIDTH);
                int height = cameras[i]->GetProp(PROPHESEE_GEOMETRY_HEIGHT);

                ShowViz(prophesee_texture, camera_viz_pos, ResizeViz(width, height, camera_viz_space),
                        (char*) camera_models[i]->GetRef(PROPHESEE_GUI_TITLE), cameras_available_show[i]);
            }
        }

        ImVec2 side_bar_pos = {ImGui::GetWindowPos().x + 0.875f * ImGui::GetWindowSize().x,
                               ImGui::GetWindowPos().y + 0.5f * (ImGui::GetWindowSize().y + menu_bar_size.y)};
        ImVec2 side_bar_size = {ImGui::GetWindowSize().x * 0.229f, (ImGui::GetWindowSize().y - menu_bar_size.y) * 0.925f};

        ImGui::SetNextWindowPos(side_bar_pos, 0, {0.5f, 0.5f});
        ImGui::SetNextWindowSize(side_bar_size);

        ImGuiWindowFlags sidebar_flags = (ImGuiWindowFlags) ImGuiWindowFlags_NoTitleBar |
                                                            ImGuiWindowFlags_NoCollapse |
                                                            ImGuiWindowFlags_NoResize |
                                                            ImGuiWindowFlags_NoMove;
        ImGui::Begin("##Sidebar", NULL, sidebar_flags);

        // General information
        if(ImGui::CollapsingHeader("General")) {
            ImGui::Spacing();

            ImGui::Text("Application   : %s", kApplicationName);
            ImGui::Spacing();

            ImGui::Text("Version       : %s", kApplicationVersion);
            ImGui::Spacing();

            ImGui::Text("Framerate     : %.2f FPS", ImGui::GetIO().Framerate);
            ImGui::Spacing();

            ImGui::Text("Show recorder");
            ImGui::SameLine();
            ImGui::Checkbox("##ShowRecordingWindow", &recording_window_show);
            ImGui::Spacing();
        }

        // Selector
        if(ImGui::CollapsingHeader("Selector")) {
            ImGui::Spacing();

            ImGui::Text("Selected camera");
            ImGui::SameLine();
            ImGui::Combo("##AvailableCameras", &cameras_available_selected,
                         cameras_available_options, IM_ARRAYSIZE(cameras_available_options));
            ImGui::Spacing();

            ImGui::Text("Visualize      ");
            ImGui::SameLine();
            ImGui::Checkbox("##VisualizeAvailableCamera", &cameras_available_show[cameras_available_selected]);
            ImGui::Spacing();
        }

        // Camera settings
        for(int i = 0; i < cameras.size(); ++i) {
            if(i != cameras_available_selected) { continue; }
            if(camera_models[i]->Type == CAM_TYPE_BASLER) {
                if(ImGui::CollapsingHeader("Information")) {
                    ImGui::Spacing();

                    if(!cameras_available_recording[i]) {
                        ImGui::Text("Status            : Not recording");
                    } else { ImGui::Text("Status            : Recording %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]); }
                    ImGui::Spacing();

                    ImGui::Text("Identifier        : %s", (char*) camera_models[i]->GetRef(BASLER_GUI_TITLE));
                    ImGui::Spacing();

                    ImGui::Text("Serial number     : %s", (char*) camera_models[i]->GetRef(BASLER_INFO_SERIAL));
                    ImGui::Spacing();

                    ImGui::Text("Model name        : %s", (char*) camera_models[i]->GetRef(BASLER_INFO_MODEL));
                    ImGui::Spacing();

                    ImGui::Text("Vendor name       : %s", (char*) camera_models[i]->GetRef(BASLER_INFO_VENDOR));
                    ImGui::Spacing();

                    ImGui::Text("Resolution        : %d * %d", (int) cameras[i]->GetProp(BASLER_DIM_WIDTH), (int) cameras[i]->GetProp(BASLER_DIM_HEIGHT));
                    ImGui::Spacing();

                    ImGui::Text("Trigger selector  : %s", (char*) camera_models[i]->GetRef(BASLER_TRIGGER_SELECTOR));
                    ImGui::Spacing();

                    ImGui::Text("Trigger source    : %s", (char*) camera_models[i]->GetRef(BASLER_TRIGGER_SOURCE));
                    ImGui::Spacing();

                    ImGui::Text("Trigger activation: %s", (char*) camera_models[i]->GetRef(BASLER_TRIGGER_ACTIVATION));
                    ImGui::Spacing();
                }

                if(ImGui::CollapsingHeader("Recording")) {
                    ImGui::BeginDisabled(multi_camera_recording);
                    ImGui::Spacing();

                    char* output_directory_path = (char*) camera_models[i]->GetRef(BASLER_GUI_PATH);

                    ImGui::Text("Recording path");
                    ImGui::SameLine();
                    ImGui::InputTextWithHint("##OutputDirectoryPath", "Specify recording path",output_directory_path,
                                     sizeof(output_directory_path));
                    ImGui::Spacing();

                    ImGui::BeginDisabled(cameras_available_recording[cameras_available_selected]);
                    if(ImGui::Button("Start")) {
                        std::string output_directory_path_final = std::string(output_directory_path);
                        if(output_directory_path_final.back() == '/') { output_directory_path_final.pop_back(); }
                        output_directory_path_final += "/" + std::string((char*) camera_models[i]->GetRef(BASLER_GUI_TITLE));
                        boost::filesystem::create_directories(output_directory_path_final);
                        cameras[i]->StartRecording(output_directory_path_final);
                        cameras_available_recording[cameras_available_selected] = true;
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!cameras_available_recording[cameras_available_selected]);
                    if(ImGui::Button("Stop") && cameras_available_recording[cameras_available_selected]) {
                        cameras[i]->StopRecording();
                        cameras_available_recording[cameras_available_selected] = false;
                    }
                    ImGui::EndDisabled();
                    ImGui::Spacing();
                    ImGui::EndDisabled();
                }

                if(ImGui::CollapsingHeader("Settings")) {
                    ImGui::Spacing();

                    ImGui::Text("Exposure time");
                    ImGui::SameLine();
                    if(ImGui::SliderInt("##ExposureTime", (int*) camera_models[i]->GetRef(BASLER_EXPOSURE_TIME), 500, 20000)) {
                        cameras[i]->SetProp(BASLER_EXPOSURE_TIME, * ( (int*) camera_models[i]->GetRef(BASLER_EXPOSURE_TIME) ));
                    }
                    ImGui::Spacing();

                    ImGui::Text("Trigger mode ");
                    ImGui::SameLine();
                    if(ImGui::Checkbox("##TriggerMode", (bool*) camera_models[i]->GetRef(BASLER_TRIGGER_MODE))) {
                        if(* ( (bool*) camera_models[i]->GetRef(BASLER_TRIGGER_MODE) )) { cameras[i]->SetProp(BASLER_TRIGGER_MODE, BASLER_TRIGGER_MODE_ON); }
                        else { cameras[i]->SetProp(BASLER_TRIGGER_MODE, BASLER_TRIGGER_MODE_OFF); }
                    }
                    ImGui::Spacing();
                }
            }

            if(camera_models[i]->Type == CAM_TYPE_PROPHESEE) {
                if(ImGui::CollapsingHeader("Information")) {
                    ImGui::Spacing();

                    if(!cameras_available_recording[i]) {
                        ImGui::Text("Status              : Not recording");
                    } else { ImGui::Text("Status              : Recording %c", "|/-\\"[(int)(ImGui::GetTime() / 0.05f) & 3]); }
                    ImGui::Spacing();

                    ImGui::Text("Identifier          : %s", (char*) camera_models[i]->GetRef(PROPHESEE_GUI_TITLE));
                    ImGui::Spacing();

                    ImGui::Text("Serial number       : %s", (char*) camera_models[i]->GetRef(PROPHESEE_INFO_SERIAL));
                    ImGui::Spacing();

                    ImGui::Text("Integrator name     : %s", (char*) camera_models[i]->GetRef(PROPHESEE_INFO_INTEGRATOR));
                    ImGui::Spacing();

                    ImGui::Text("Plugin name         : %s", (char*) camera_models[i]->GetRef(PROPHESEE_INFO_PLUGIN));
                    ImGui::Spacing();

                    ImGui::Text("Generation          : %s", (char*) camera_models[i]->GetRef(PROPHESEE_INFO_GENERATION));
                    ImGui::Spacing();

                    ImGui::Text("Raw formats         : %s", (char*) camera_models[i]->GetRef(PROPHESEE_INFO_RAW_FORMATS));
                    ImGui::Spacing();

                    ImGui::Text("Resolution          : %d * %d", (int) cameras[i]->GetProp(PROPHESEE_GEOMETRY_WIDTH), (int) cameras[i]->GetProp(PROPHESEE_GEOMETRY_HEIGHT));
                    ImGui::Spacing();

                    ImGui::Text("Synchronization mode: %s", (char*) camera_models[i]->GetRef(PROPHESEE_SYNC_MODE));
                    ImGui::Spacing();
                }

                if(ImGui::CollapsingHeader("Recording")) {
                    ImGui::BeginDisabled(multi_camera_recording);
                    ImGui::Spacing();

                    char* output_directory_path = (char*) camera_models[i]->GetRef(PROPHESEE_GUI_PATH);

                    ImGui::Text("Recording path");
                    ImGui::SameLine();
                    ImGui::InputTextWithHint("##OutputDirectoryPath", "Specify recording path",output_directory_path,
                                     sizeof(output_directory_path));
                    ImGui::Spacing();

                    ImGui::BeginDisabled(cameras_available_recording[cameras_available_selected]);
                    if(ImGui::Button("Start")) {
                        std::string output_directory_path_final = std::string(output_directory_path);
                        if(output_directory_path_final.back() == '/') { output_directory_path_final.pop_back(); }
                        output_directory_path_final += "/" + std::string((char*) camera_models[i]->GetRef(PROPHESEE_GUI_TITLE));
                        boost::filesystem::create_directories(output_directory_path_final);
                        cameras[i]->StartRecording(output_directory_path_final);
                        cameras_available_recording[cameras_available_selected] = true;
                    }
                    ImGui::EndDisabled();
                    ImGui::SameLine();
                    ImGui::BeginDisabled(!cameras_available_recording[cameras_available_selected]);
                    if(ImGui::Button("Stop") && cameras_available_recording[cameras_available_selected]) {
                        cameras[i]->StopRecording();
                        cameras_available_recording[cameras_available_selected] = false;
                    }
                    ImGui::EndDisabled();
                    ImGui::Spacing();
                    ImGui::EndDisabled();
                }

                if(ImGui::CollapsingHeader("Settings")) {
                    ImGui::Spacing();

                    ImGui::Text("bias_diff_off");
                    ImGui::SameLine();
                    if(ImGui::SliderInt("##BiasDiffOff", (int*) camera_models[i]->GetRef(PROPHESEE_BIAS_DIFF_OFF),
                                        cameras[i]->GetProp(PROPHESEE_BIAS_DIFF_OFF_MIN), cameras[i]->GetProp(PROPHESEE_BIAS_DIFF_OFF_MAX))) {
                        cameras[i]->SetProp(PROPHESEE_BIAS_DIFF_OFF, * ((int*) camera_models[i]->GetRef(PROPHESEE_BIAS_DIFF_OFF)));
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_diff_on ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt("##BiasDiffOn", (int*) camera_models[i]->GetRef(PROPHESEE_BIAS_DIFF_ON),
                                        cameras[i]->GetProp(PROPHESEE_BIAS_DIFF_ON_MIN), cameras[i]->GetProp(PROPHESEE_BIAS_DIFF_ON_MAX))) {
                        cameras[i]->SetProp(PROPHESEE_BIAS_DIFF_ON, * ((int*) camera_models[i]->GetRef(PROPHESEE_BIAS_DIFF_ON)));
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_fo      ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt("##BiasFo", (int*) camera_models[i]->GetRef(PROPHESEE_BIAS_FO),
                                        cameras[i]->GetProp(PROPHESEE_BIAS_FO_MIN), cameras[i]->GetProp(PROPHESEE_BIAS_FO_MAX))) {
                        cameras[i]->SetProp(PROPHESEE_BIAS_FO, * ((int*) camera_models[i]->GetRef(PROPHESEE_BIAS_FO)));
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_hpf     ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt("##BiasHpf", (int*) camera_models[i]->GetRef(PROPHESEE_BIAS_HPF),
                                        cameras[i]->GetProp(PROPHESEE_BIAS_HPF_MIN), cameras[i]->GetProp(PROPHESEE_BIAS_HPF_MAX))) {
                        cameras[i]->SetProp(PROPHESEE_BIAS_HPF, * ((int*) camera_models[i]->GetRef(PROPHESEE_BIAS_HPF)));
                    }
                    ImGui::Spacing();

                    ImGui::Text("bias_refr    ");
                    ImGui::SameLine();
                    if(ImGui::SliderInt("##BiasRefr", (int*) camera_models[i]->GetRef(PROPHESEE_BIAS_REFR),
                                        cameras[i]->GetProp(PROPHESEE_BIAS_REFR_MIN), cameras[i]->GetProp(PROPHESEE_BIAS_REFR_MAX))) {
                        cameras[i]->SetProp(PROPHESEE_BIAS_REFR, * ((int*) camera_models[i]->GetRef(PROPHESEE_BIAS_REFR)));
                    }
                    ImGui::Spacing();
                }
            }
        }

        ImGui::End();

        // Recording window
        if(recording_window_show) {
            ImVec2 main_viewport_pos = ImGui::GetMainViewport()->Pos;
            ImVec2 display_size = ImGui::GetIO().DisplaySize;
            ImGui::SetNextWindowPos({main_viewport_pos.x + 0.5f * display_size.x, main_viewport_pos.y + 0.5f * display_size.y}, ImGuiCond_Appearing,
                                    {0.5f, 0.5f});
            ImGui::SetNextWindowSize({display_size.x * 0.20f, display_size.y * 0.45f});
            window_flags = (ImGuiWindowFlags) ImGuiWindowFlags_NoCollapse |
                                              ImGuiWindowFlags_NoResize;
            ImGui::Begin("Recorder", &recording_window_show, window_flags);

            ImGui::Spacing();

            ImGui::Text("Identifier");
            ImGui::SameLine();
            ImGui::InputTextWithHint("##CustomRecordingIdentifier", "Enter recording identifier (opt.)", custom_recording_identifier, sizeof(custom_recording_identifier));
            ImGui::Spacing();

            const char* recording_options_arr[recordings.size() + 1];
            recording_options_arr[0] = "Select recording";

            int j = 1;
            for(const auto& recording : recordings) {
                recording_options_arr[j] = recording.first.c_str();
                ++j;
            }

            ImGui::Text("Recording ");
            ImGui::SameLine();
            if(ImGui::Combo("##RecordingSelector",
                         &recording_selected,
                         recording_options_arr,
                         IM_ARRAYSIZE(recording_options_arr)) &&
                         recording_selected != 0) {
                std::string recording_path_selected = std::string(recording_options_arr[recording_selected]);
                for(const auto& i : recordings[recording_path_selected]) {
                    if(camera_models[i]->Type == CAM_TYPE_BASLER) {
                        cv::VideoCapture video_capture {std::string(recording_path_selected) + "/" + std::string((char*) camera_models[i]->GetRef(BASLER_GUI_TITLE)) + "/frames.mp4"};
                        int frame_count = video_capture.get(cv::CAP_PROP_FRAME_COUNT);
                        camera_models[i]->SetRef(BASLER_GUI_FRAMES, &frame_count);
                    }

                    if(camera_models[i]->Type == CAM_TYPE_PROPHESEE) {
                        std::fstream trigger_events;
                        trigger_events.open(std::string(recording_path_selected) + "/" + std::string((char*) camera_models[i]->GetRef(PROPHESEE_GUI_TITLE)) + "/trigger_events.txt", std::ios::in);

                        int ext_trigger_event_count = 0;
                        std::string trigger_event;
                        while(std::getline(trigger_events, trigger_event)) {
                            ++ext_trigger_event_count;
                        }

                        camera_models[i]->SetRef(PROPHESEE_GUI_TRIGGERS, &ext_trigger_event_count);
                    }
                }
            }
            ImGui::Spacing();

            if(recording_selected != 0) {
                if(ImGui::BeginTabBar("Recordings")) {
                    for(const auto &i: recordings[std::string(recording_options_arr[recording_selected])]) {
                        if(camera_models[i]->Type == CAM_TYPE_BASLER) {
                            if(ImGui::BeginTabItem((char *) camera_models[i]->GetRef(BASLER_GUI_TITLE))) {
                                ImGui::Spacing();

                                ImGui::Text("Frame count: %d", *((int*) camera_models[i]->GetRef(BASLER_GUI_FRAMES)));
                                ImGui::Spacing();

                                ImGui::EndTabItem();
                            }
                        }

                        if(camera_models[i]->Type == CAM_TYPE_PROPHESEE) {
                            if(ImGui::BeginTabItem((char *) camera_models[i]->GetRef(PROPHESEE_GUI_TITLE))) {
                                ImGui::Spacing();

                                ImGui::Text("Ext. trigger events: %d", *((int*) camera_models[i]->GetRef(PROPHESEE_GUI_TRIGGERS)));
                                ImGui::Spacing();

                                ImGui::EndTabItem();
                            }
                        }
                    }

                    ImGui::EndTabBar();
                }
            }

            float next_cursor_pos_y = std::max(ImGui::GetWindowContentRegionMax().y * (1 - 0.0501265f), ImGui::GetCursorPos().y);
            ImGui::SetCursorPosY(next_cursor_pos_y);

            bool camera_available_recording = false;
            for(int i = 0; i < cameras.size(); ++i) {
                if(cameras_available_recording[i]) { camera_available_recording = true; break; }
            }

            bool camera_available_not_recording = false;
            for(int i = 0; i < cameras.size(); ++i) {
                if(!cameras_available_recording[i]) { camera_available_not_recording = true; break; }
            }

            ImGui::BeginDisabled(camera_available_recording);
            if(ImGui::Button("Record")) {
                time_t raw_time;
                struct tm* time_info;
                char time_stamp[64];

                time(&raw_time);
                time_info = localtime(&raw_time);

                strftime(time_stamp, sizeof(time_stamp), "%y%m%d_%H%M%S", time_info);

                std::string recording_directory_path_final = std::string(time_stamp);
                if(!std::string(custom_recording_identifier).empty()) { recording_directory_path_final += "_" + std::string(custom_recording_identifier); }
                boost::filesystem::create_directories(recording_directory_path_final);

                std::vector<int> camera_indices;
                for(int i = 0; i < cameras.size(); ++i) {
                    if(camera_models[i]->Type == CAM_TYPE_BASLER) {
                        std::string basler_recording_directory_path = recording_directory_path_final + "/" + std::string((char*) camera_models[i]->GetRef(BASLER_GUI_TITLE));
                        boost::filesystem::create_directories(basler_recording_directory_path);
                        cameras[i]->StartRecording(basler_recording_directory_path);
                    }

                    if(camera_models[i]->Type == CAM_TYPE_PROPHESEE) {
                        std::string prophesee_recording_directory_path = recording_directory_path_final + "/" + std::string((char*) camera_models[i]->GetRef(PROPHESEE_GUI_TITLE));
                        boost::filesystem::create_directories(prophesee_recording_directory_path);
                        cameras[i]->StartRecording(prophesee_recording_directory_path);
                    }

                    cameras_available_recording[i] = true;
                    camera_indices.push_back(i);
                }

                multi_camera_recording = true;
                current_recording = {recording_directory_path_final, camera_indices};
            }
            ImGui::EndDisabled();
            ImGui::SameLine();
            ImGui::BeginDisabled(camera_available_not_recording);
            if(ImGui::Button("Finish")) {
                for(int i = 0; i < cameras.size(); ++i) {
                    cameras[i]->StopRecording();
                    cameras_available_recording[i] = false;
                }

                multi_camera_recording = false;
                recordings.insert(current_recording);
            }
            ImGui::EndDisabled();
            ImGui::Spacing();

            ImGui::End();
        }

        ImGui::End();
        EndFrame(win, clear_color);
    }

    for(auto texture : textures) { DeleteTexture(texture); }
}

int main() {
    #ifdef ENABLE_PYLON_SDK
    camera_launchers.push_back(std::make_unique<BaslerLauncher>());
    #endif //ENABLE_PYLON_SDK

    #ifdef ENABLE_METAVISION_SDK
    camera_launchers.push_back(std::make_unique<PropheseeLauncher>());
    #endif //ENABLE_METAVISION_SDK

    Init();
    ImGui::StyleColorsLight();

    if(CocaptureLauncher()) {
        CocaptureApplication();
    }

    DestroyWindow(win);
    Exit();
}

#endif //ENABLE_DEAR_IMGUI