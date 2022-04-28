cmake_minimum_required(VERSION 3.22)
project(rip_cocapture_gui)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)

find_package(Pangolin 0.4 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

set(MetavisionSDK_DIR "/home/friedhelm/env/openeb/build/generated/share/cmake/MetavisionSDKCMakePackagesFilesDir")
set(MetavisionHAL_DIR "/home/friedhelm/env/openeb/build/generated/share/cmake/MetavisionHALCMakePackagesFilesDir")
find_package(MetavisionSDK COMPONENTS core driver ui REQUIRED)



add_executable(${PROJECT_NAME} app/main.cpp lib/prophesee_interface.cpp lib/prophesee_interface.h lib/event_visualizer.cpp lib/event_visualizer.h)
target_link_libraries(${PROJECT_NAME}
        pango_display
        ${OpenCV_LIBRARIES}
        MetavisionSDK::core
        MetavisionSDK::driver
        MetavisionSDK::ui
        Metavision::HAL_discovery)
