# rip-cocapture-gui
Light-weight viewer and recorder for the co-capture system

## Dependencies:

- OpenEB, from https://github.com/prophesee-ai/openeb
- Pylon, from https://www.baslerweb.com/de/downloads/downloads-software/
- Pangolin, from https://github.com/stevenlovegrove/Pangolin

## Installation

Tested on Ubuntu 20.04

    mkdir build && cd build
    cmake ..
    make -j

## Usage

Start recorder with

    ./recorder

To warp basler frames use

    ./recorder --warp --homography <PATH-TO-HOMOGRAPHY>

The homography is given as text file as seen in the example config/homography.txt
