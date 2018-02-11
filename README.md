# chip8-emulator

This program is an emulator for the chip 8. It is able to load and play chip 8 ROMs.
The ROMs are not provided here, you can easily find them on the web if you want.

## Prerequisites

You need the following to build this project:
* Windows (tested and developed on Windows 10)
* CMake
* Visual Studio (tested and developed using Visual Studio 2017 Community)

## Usage

* Copy your ROMs to the games folder
* Run cmd and navigate to the sln folder
* Run "cmake .."
* Open solution in Visual Studio
* Build and run
* When the emulator is opened it will wait for you to type the name of the ROM you want to play

## License

### SDL2

This project uses SDL2, which is provided in this repository. You can find the license [here](https://www.libsdl.org/license.php).

### FindSDL2.cmake

FindSDL2.cmake is a script that I found [here](https://github.com/tcbrindle/sdl2-cmake-scripts).
His modifications are released under the two-clause BSD license.
However, the original file is released under the three-clause BSD license and most likely originates from [here](https://github.com/Kitware/VTK)

### chip8-emulator

The rest of the files are written by me and are released under the three-clause BSD License, which can be found in License.txt.