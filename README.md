# rip-cocapture-gui

Light-weight viewer and recorder for the co-capture system

![Screenshot from 2023-07-05 10-55-54](https://github.com/tub-rip/rip-cocapture-gui/assets/115785598/197ac573-fb37-41e3-b911-8c1f4081a7db)

## Dependencies

### Platform and graphics dependencies:

Install OpenGL3 library with:

    sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev  

Install SDL2 library with:

    sudo apt-get install libsdl2-dev

### Dependencies for the cameras:

- OpenEB, from https://github.com/prophesee-ai/openeb
    - Version 4.2.0 is required

- Pylon, from https://www.baslerweb.com/de/downloads/downloads-software/
    - Version 7.2.1 is required

### Further dependencies:
- OpenCV, from https://github.com/opencv/opencv
- Boost, from https://www.boost.org/users/download/

## Usage

* Build project with:

        mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make

    Note that you can accelerate this process by specifying the number of jobs ran in parallel during the build with -j 

* Display help message with:

        ./main --help or ./main -h

* List connected cameras with:

        ./main --list or ./main -l

* Select cameras during startup with:

        ./main --pick INDEX or ./main -p INDEX

    Where ```INDEX``` denotes the camera index given by the output of ```./main --list``` or ```./main -l```
