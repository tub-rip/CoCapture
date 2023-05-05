cmake_minimum_required(VERSION 3.22)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(Cocapture VERSION 1.0)

### Cocapture
add_library(Cocapture STATIC)
target_include_directories(Cocapture PUBLIC core)

### Enable dependencies here
set(USE_DEAR_IMGUI TRUE)
set(USE_PYLON_SDK TRUE)
set(USE_METAVISION_SDK TRUE)

### Dear ImGui
if(USE_DEAR_IMGUI)
    add_definitions(-DENABLE_DEAR_IMGUI)

    add_library(DearImGui STATIC)
    find_package(SDL2 REQUIRED)
    find_package(OpenGL REQUIRED)

    set(IMGUI_SRC_DIR lib/imgui)
    file(GLOB_RECURSE IMGUI_SRC_FILES ${IMGUI_SRC_DIR}/*.cpp)
    target_sources(DearImGui PUBLIC ${IMGUI_SRC_FILES})
    set(IMGUI_INCLUDE_DIRS ${IMGUI_SRC_DIR} ${IMGUI_SRC_DIR}/backend)

    target_include_directories(DearImGui PUBLIC
            ${IMGUI_INCLUDE_DIRS}
            ${SDL2_INCLUDE_DIRS}
            ${OPENGL_INCLUDE_DIR})

    target_link_libraries(DearImGui PUBLIC
            ${CMAKE_DL_LIBS}
            ${SDL2_LIBRARIES}
            ${OPENGL_LIBRARIES})

    target_link_libraries(Cocapture PUBLIC DearImGui)
    target_sources(Cocapture PUBLIC core/gui/gui.cpp)
    target_include_directories(Cocapture PUBLIC core/gui)

    add_executable(cocapture src/cocapture.cpp)
    target_link_libraries(cocapture PUBLIC Cocapture)
endif()

### PylonSDK
if(USE_PYLON_SDK)
    add_definitions(-DENABLE_PYLON_SDK)
    find_package(pylon 7.2.1 REQUIRED)
    target_link_libraries(Cocapture PUBLIC pylon::pylon)

    target_sources(Cocapture PUBLIC core/basler/basler_launcher.cpp)
    target_sources(Cocapture PUBLIC core/basler/basler_camera.cpp)
    target_include_directories(Cocapture PUBLIC core/basler)
endif()

### MetavisionSDK
if(USE_METAVISION_SDK)
    add_definitions(-DENABLE_METAVISION_SDK)
    find_package(MetavisionSDK COMPONENTS core driver REQUIRED)
    target_link_libraries(Cocapture PUBLIC MetavisionSDK::core MetavisionSDK::driver Metavision::HAL_discovery)

    target_sources(Cocapture PUBLIC core/prophesee/prophesee_launcher.cpp)
    target_sources(Cocapture PUBLIC core/prophesee/prophesee_camera.cpp)
    target_include_directories(Cocapture PUBLIC core/prophesee)
endif()

### OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(Cocapture PUBLIC ${OpenCV_LIBRARIES})

### Boost
if(USE_DEAR_IMGUI)
    find_package(Boost COMPONENTS filesystem REQUIRED)
    target_link_libraries(Cocapture PUBLIC Boost::filesystem)
endif()