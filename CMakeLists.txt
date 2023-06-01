cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### rip-cocapture-gui
project(rip-cocapture-gui)
add_library(rip-cocapture-gui STATIC)

### Recorder
add_executable(recorder src/main.cpp)
target_link_libraries(recorder PUBLIC rip-cocapture-gui)

### Enable dependencies here
set(USE_METAVISION_SDK TRUE)
set(USE_REALSENSE_SDK TRUE)

### Metavision SDK
if(USE_METAVISION_SDK)
    add_definitions(-DENABLE_METAVISION_SDK)
    find_package(MetavisionSDK COMPONENTS core driver REQUIRED)
    target_link_libraries(rip-cocapture-gui PUBLIC MetavisionSDK::core MetavisionSDK::driver Metavision::HAL_discovery)
    target_sources(rip-cocapture-gui PUBLIC cams/prophesee/prophesee_camera.cpp gui/prophesee/prophesee_gui.cpp)
    target_include_directories(rip-cocapture-gui PUBLIC cams/prophesee gui/prophesee)
endif()

### RealSense SDK
if(USE_REALSENSE_SDK)
    add_definitions(-DENABLE_REALSENSE_SDK)
    find_package(realsense2 REQUIRED)
    target_include_directories(rip-cocapture-gui PUBLIC ${realsense_INCLUDE_DIR})
    target_link_libraries(rip-cocapture-gui PUBLIC ${realsense2_LIBRARY})
    target_sources(rip-cocapture-gui PUBLIC cams/realsense/realsense_camera.cpp gui/realsense/realsense_gui.cpp)
    target_include_directories(rip-cocapture-gui PUBLIC cams/realsense gui/realsense)
endif()

### Dear ImGui
add_library(DearImGui STATIC)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

set(IMGUI_SRC_DIR lib/imgui)
file(GLOB_RECURSE IMGUI_SRC_FILES ${IMGUI_SRC_DIR}/*.cpp)
target_sources(DearImGui PUBLIC ${IMGUI_SRC_FILES})
set(IMGUI_INCLUDE_DIRS ${IMGUI_SRC_DIR} ${IMGUI_SRC_DIR}/backends)

target_include_directories(DearImGui PUBLIC
        ${IMGUI_INCLUDE_DIRS}
        ${SDL2_INCLUDE_DIRS}
        ${OPENGL_INCLUDE_DIR})

target_link_libraries(DearImGui PUBLIC
        ${CMAKE_DL_LIBS}
        ${SDL2_LIBRARIES}
        ${OPENGL_LIBRARIES})

target_link_libraries(rip-cocapture-gui PUBLIC DearImGui)
target_sources(rip-cocapture-gui PUBLIC gui/imgui_helper.cpp)
target_include_directories(rip-cocapture-gui PUBLIC gui)

### OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(rip-cocapture-gui PUBLIC ${OpenCV_LIBRARIES})

### Boost
find_package(Boost COMPONENTS filesystem REQUIRED)
target_link_libraries(rip-cocapture-gui PUBLIC Boost::filesystem)