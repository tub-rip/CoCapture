cmake_minimum_required(VERSION 3.22)
project(rip_cocapture_gui)
set(CMAKE_CXX_STANDARD 17)

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

set(boost_components_to_find program_options filesystem timer chrono thread)
find_package(Boost COMPONENTS ${boost_components_to_find} REQUIRED)

add_library(ImGui STATIC)
find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

target_sources(ImGui
		       PRIVATE
        lib/Gui/imgui/imgui.cpp lib/Gui/imgui/imgui_draw.cpp
        lib/Gui/imgui/imgui_tables.cpp lib/Gui/imgui/imgui_widgets.cpp
        lib/Gui/imgui/backend/imgui_impl_sdl.cpp lib/Gui/imgui/backend/imgui_impl_opengl3.cpp
        lib/Gui/imgui/imgui_demo.cpp)

target_include_directories(ImGui
						   PUBLIC
        lib/Gui/imgui lib/Gui/imgui/backend
		  				   ${OPENGL_INCLUDE_DIR} ${SDL2_INCLUDE_DIRS})

target_link_libraries(ImGui
					  PUBLIC
					  ${CMAKE_DL_LIBS}
					  ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES})

find_package(MetavisionSDK COMPONENTS core driver ui REQUIRED)

find_package(Pylon QUIET)
if (NOT ${Pylon_FOUND})
    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/FindPylon.cmake")
endif ()

include_directories(
        ${Pylon_INCLUDE_DIRS}
)

add_executable(recorder
        app/recorder.cpp
        lib/basler_event_handler.h lib/utils.h lib/parameters.h lib/constants.h lib/camera/base.cpp
        lib/camera/base.h lib/camera/prophesee_cam.cpp lib/camera/prophesee_cam.h lib/constants.cpp
        lib/utils.cpp lib/camera/basler_camera.cpp lib/camera/basler_camera.h lib/Gui/gui.cpp lib/Gui/gui.h
		lib/Gui/Base.cpp lib/Gui/Base.h
		lib/Gui/CameraController.cpp lib/Gui/CameraController.h
		lib/Gui/BaslerWrapper.h
		lib/Gui/Constants.h)

target_link_libraries(recorder
		ImGui
        ${OpenCV_LIBRARIES}
        Boost::program_options
        MetavisionSDK::core
        MetavisionSDK::driver
        MetavisionSDK::ui
        Metavision::HAL_discovery
        Threads::Threads
        ${Pylon_LIBRARIES})