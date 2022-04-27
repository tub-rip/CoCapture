cmake_minimum_required(VERSION 3.22)
project(rip_cocapture_gui)
set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
find_package(Pangolin 0.4 REQUIRED)
include_directories(${Pangolin_INCLUDE_DIRS})


add_executable(${PROJECT_NAME} app/main.cpp)
target_link_libraries(${PROJECT_NAME}
        pango_display
        ${OpenCV_LIBRARIES})
