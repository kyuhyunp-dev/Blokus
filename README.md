# Blokus (C++ / SFML / GoogleTest)
Blokus game, built in C++ using the [SFML](https://github.com/SFML/SFML) library, unit-tested with [GoogleTest](https://github.com/google/googletest). Continuous integration using a basic GitHub action. 

## Project Design

## Project Features

### Features To Build
- Move pieces with the mouse
- Game Tutorial
- One vs one on the same computer

## Gameplay

## Run Blokus
[CMake](https://cmake.org/download/) is used to configure and build (note that you may need to download `ninja`). On Linux, install SFML's dependencies using your system package manager. On Ubuntu and other Debian-based distributions, you can use the following commands:
```
sudo apt update
sudo apt install \
    libxrandr-dev \
    libxcursor-dev \
    libxi-dev \
    libudev-dev \
    libfreetype-dev \
    libflac-dev \
    libvorbis-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
    libfreetype-dev
```


### Terminal

#### Configure
```
mkdir build
cd build
cmake ..
```
#### Build
```
cmake --build .
```
#### Run Tests
```
./bin/BlokusTests
```
#### Run Game
```
./bin/Blokus
```
### Visual Studio
`Ctrl+S` to configure
Click `Build` in the menu bar and click `Build All`
Run tests by choosing `BlokusTests` and the play button
Run the app by selecting `Blokus` and the play button


## Project Management
Tracked progress using Github's [Backlog](https://github.com/orgs/kyuhyunp-dev/projects/1)





