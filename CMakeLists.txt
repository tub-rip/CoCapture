cmake_minimum_required(VERSION 3.22)
project(rip_cocapture_gui)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

find_package(Pangolin 0.4 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})

set(MetavisionSDK_DIR "/home/friedhelm/env/openeb/build/generated/share/cmake/MetavisionSDKCMakePackagesFilesDir")
set(MetavisionHAL_DIR "/home/friedhelm/env/openeb/build/generated/share/cmake/MetavisionHALCMakePackagesFilesDir")
find_package(MetavisionSDK COMPONENTS core driver ui REQUIRED)

find_package(Pylon QUIET)
if (NOT ${Pylon_FOUND})
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPylon.cmake")
endif()

include_directories(
        ${Pylon_INCLUDE_DIRS}
)

add_executable(recorder
        app/recorder.cpp
        lib/event_visualizer.cpp
        lib/event_visualizer.h
        lib/prophesee_interface.cpp
        lib/prophesee_interface.h
        lib/basler_event_handler.cpp
        lib/basler_event_handler.h)

target_link_libraries(recorder
        pango_display
        ${OpenCV_LIBRARIES}
        MetavisionSDK::core
        MetavisionSDK::driver
        MetavisionSDK::ui
        Metavision::HAL_discovery
        Threads::Threads
        ${Pylon_LIBRARIES})