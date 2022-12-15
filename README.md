# rip-cocapture-gui

Light-weight viewer and recorder for the co-capture system

## Dependencies:

Install sdl2 library with:

    sudo apt-get install libsdl2-dev

further dependencies for the cameras:

- OpenEB, from https://github.com/prophesee-ai/openeb
- Pylon, from https://www.baslerweb.com/de/downloads/downloads-software/

## Installation

Tested on Ubuntu 20.04

    mkdir build && cd build
    cmake ..
    make -j

## Usage

Start recorder with

    ./recorder --cameras <type1> <type2> ...

where `<typeX>` is either `basler` or `prophesee`. So for two Prophesee cameras call:

    ./recorder --cameras prophesee prophesee

Use the flag `--record` to record from startup. For synchronized data streams
the cameras need to be hardware triggered.