cmake_minimum_required(VERSION 3.22)
project(rip_cocapture_gui)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)

find_package(Pangolin 0.4 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

set(MetavisionSDK_DIR "/home/friedhelm/env/openeb/build/generated/share/cmake/MetavisionSDKCMakePackagesFilesDir")
set(MetavisionHAL_DIR "/home/friedhelm/env/openeb/build/generated/share/cmake/MetavisionHALCMakePackagesFilesDir")
find_package(MetavisionSDK COMPONENTS core driver ui REQUIRED)

find_package(Threads REQUIRED)


add_executable(recorder app/recorder.cpp lib/event_visualizer.cpp lib/event_visualizer.h lib/prophesee_interface.cpp lib/prophesee_interface.h)
target_link_libraries(recorder
        pango_display
        ${OpenCV_LIBRARIES}
        MetavisionSDK::core
        MetavisionSDK::driver
        MetavisionSDK::ui
        Metavision::HAL_discovery
        Threads::Threads)

add_executable(visualizer app/visualizer.cpp lib/event_queue.cpp lib/event_queue.h)
target_link_libraries(visualizer
        MetavisionSDK::core
        MetavisionSDK::driver
)
