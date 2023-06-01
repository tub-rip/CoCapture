# rip-cocapture-gui

Light-weight viewer and recorder for the co-capture system

## Dependencies:

Install OpenGL3 library with:

    sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev  

Install SDL2 library with:

    sudo apt-get install libsdl2-dev

further dependencies for the cameras:

- OpenEB, from https://github.com/prophesee-ai/openeb
    - Tested versions are: 2.3.0, 3.0.2
      (Please, note that we do not support 4.0 at the moment, as it introduces some breaking changes.)
- Pylon, from https://www.baslerweb.com/de/downloads/downloads-software/
    - Tested versions are: 7.2.1
- RealSense, from https://dev.intelrealsense.com/docs/installation
    - Tested versions are: 2.0


## Installation

Tested on Ubuntu 20.04

    mkdir build && cd build
    cmake ..
    make -j

## Usage

- TODO