cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

### rip-cocapture-gui
project(rip-cocapture-gui)
add_library(rip-cocapture-gui STATIC)
target_include_directories(rip-cocapture-gui PUBLIC cams gui cam_interfaces wrappers)

### main
add_executable(main src/main.cpp)
target_link_libraries(main PUBLIC rip-cocapture-gui)

### tools
target_sources(rip-cocapture-gui PUBLIC tools/recorder/recorder.cpp)
target_include_directories(rip-cocapture-gui PUBLIC tools/recorder)

### Enable dependencies here
set(USE_METAVISION_SDK TRUE) # for Prophesee cameras
set(USE_PYLON_SDK FALSE) # for Basler cameras
set(USE_SPINNAKER_SDK FALSE) # for Flir cameras

if (USE_METAVISION_SDK)
    add_definitions(-DENABLE_METAVISION_SDK)
    find_package(MetavisionSDK COMPONENTS core driver REQUIRED)
    target_link_libraries(rip-cocapture-gui PUBLIC MetavisionSDK::core MetavisionSDK::driver Metavision::HAL_discovery)
    target_sources(rip-cocapture-gui PUBLIC cams/prophesee/prophesee_camera.cpp cam_interfaces/prophesee/prophesee_interface.cpp)
    target_sources(rip-cocapture-gui PUBLIC gui/prophesee/prophesee_gui.cpp wrappers/prophesee/prophesee_wrapper.cpp)
    target_include_directories(rip-cocapture-gui PUBLIC cams/prophesee gui/prophesee cam_interfaces/prophesee wrappers/prophesee)
endif ()

if (USE_PYLON_SDK)
    add_definitions(-DENABLE_PYLON_SDK)
    find_package(pylon REQUIRED COMPONENTS 7.2.0)
    target_link_libraries(rip-cocapture-gui PUBLIC pylon::pylon)
    target_sources(rip-cocapture-gui PUBLIC cams/basler/basler_camera.cpp cam_interfaces/basler/basler_interface.cpp)
    target_sources(rip-cocapture-gui PUBLIC gui/basler/basler_gui.cpp wrappers/basler/basler_wrapper.cpp)
    target_include_directories(rip-cocapture-gui PUBLIC cams/basler gui/basler cam_interfaces/basler wrappers/basler)
endif ()

if (USE_SPINNAKER_SDK)
    add_definitions(-DENABLE_SPINNAKER_SDK)
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}")
    find_package(Spinnaker REQUIRED)
    target_link_libraries(rip-cocapture-gui PUBLIC ${Spinnaker_LIBRARIES})
    target_sources(rip-cocapture-gui PUBLIC cams/flir/flir_camera.cpp cam_interfaces/flir/flir_interface.cpp)
    target_sources(rip-cocapture-gui PUBLIC gui/flir/flir_gui.cpp wrappers/flir/flir_wrapper.cpp)
    target_include_directories(rip-cocapture-gui PUBLIC cams/flir gui/flir cam_interfaces/flir wrappers/flir)

    find_package(OpenMP)
    if(OpenMP_CXX_FOUND)
        target_link_libraries(rip-cocapture-gui PUBLIC OpenMP::OpenMP_CXX)
    endif()
endif ()


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
find_package(Boost COMPONENTS filesystem program_options REQUIRED)
target_link_libraries(rip-cocapture-gui PUBLIC Boost::filesystem Boost::program_options)