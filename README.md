# CoCapture

GUI for viewing and recording with multi-camera systems including event cameras.

![Screenshot from 2023-07-05 10-55-54](docs/images/cocapture_example.png)

## Installation

Tested on Ubuntu 20.04

### 1. Dependencies
#### Platform and graphics dependencies:

Install OpenGL3 library with:

    sudo apt-get install libglu1-mesa-dev freeglut3-dev mesa-common-dev  

Install SDL2 library with:

    sudo apt-get install libsdl2-dev

#### Dependencies for the cameras:

Below are links for the dependencies required for different cameras. You only need to install the dependencies for the cameras you are using. You have to 'deactivate' linking unused libraries by setting the flags in the CMakeLists.txt, e.g. change `set(USE_METAVISION_SDK TRUE)` to `set(USE_METAVISION_SDK FALSE)`, if you do not want to use Prophesee cameras.

- [OpenEB](https://github.com/prophesee-ai/openeb): tested version: 4.2.0, (>4 required)

- [Pylon](https://www.baslerweb.com/de/downloads/downloads-software/): tested version: 7.2.0, 7.2.1

- [Spinnaker](https://www.flir.com/products/spinnaker-sdk/): tested Version: 3.1.0.79

#### Further dependencies:
- [OpenCV](https://github.com/opencv/opencv)
- [Boost](https://www.boost.org/users/download/)

You can also install the packages from apt if applicable.

### 2. Compilation

* Build project with:

        mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make

## Run

* Display help message with:

        ./main --help or ./main -h

* List connected cameras with:

        ./main --list or ./main -l

* Select cameras during startup with:

        ./main --pick INDEX or ./main -p INDEX

    Where ```INDEX``` denotes the camera index given by the output of ```./main --list``` or ```./main -l```

## Publications and Citation

Publications using CoCapture are recorded in [Publications](./docs/publications.md). If you use CoCapture for your paper, you can also add it to this table by a pull request.

If you use CoCapture for your research, please consider citing it as follows:

```
@misc{rip-cocapture-gui,
	title = {CoCapture},
	author = {Hamann, Friedhelm and Febryanto, Filbert and Ghosh, Suman and Gallego, Guillermo},
	year = {2023},
	howpublished = {\url{https://github.com/tub-rip/rip-cocapture-gui}},
	note = {Accessed: YYYY-MM-DD},
}
```

Note: To specify the version of the software you are using, the default value YYYY-MM-DD in the note field should be replaced with the date of the last change of the software you are using, i.e. the date of the latest commit.

## Contribution

Contributions are very welcome, for example, plugins for other camera vendors or open issues. Just make a PR.
