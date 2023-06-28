# rip-cocapture-gui

Light-weight viewer and recorder for the co-capture system

## Dependencies

### Platform and graphics dependencies:

Install OpenGL3 library with:

    sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev  

Install SDL2 library with:

    sudo apt-get install libsdl2-dev

### Dependencies for the cameras:

- OpenEB, from https://github.com/prophesee-ai/openeb
    - Tested versions are: 2.3.0, 3.0.2
      (Please, note that we do not support 4.0 at the moment, as it introduces some breaking changes.)

- Pylon, from https://www.baslerweb.com/de/downloads/downloads-software/
    - Version 7.2.1 is required

### Further dependencies:
- OpenCV, from https://github.com/opencv/opencv
- Boost, from https://www.boost.org/users/download/
