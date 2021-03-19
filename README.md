[![Build Status](https://www.travis-ci.com/emulated-gamer-bois/gameboy-emulator.svg?branch=dev)](https://www.travis-ci.com/emulated-gamer-bois/gameboy-emulator)

# Game Boy Emulator

This emulator was built as a Bachelor's project at Chalmers University of Technology by the students listed as contributors below. The emulator aims to emulate the original Game Boy released in 1989.

## Contributors

- David Möller  [@molleer](https://github.com/molleer)
- Isak Lindgren [@isaaklindgren](https://github.com/isaaklindgren)
- Algot Axelzon [@AlgotAxelzon](https://github.com/isaaklindgren)
- Arvid Rydberg [@Riddarvid](https://github.com/Riddarvid)
- Andreas Palmqvist [@tehAndrew](https://github.com/tehAndrew)
- Carl Lindh [@Kviick](https://github.com/Kviick)

### Supervisor

- Roc R. Currius [@roc-r-currius](https://github.com/roc-r-currius)

## Setup

Install the required dependencies `sdl`, `sdl2` and `glew`.

Ex:
```
sudo pacman -S sdl sdl2 glew
```
or
```
sudo apt-get install libsdl2-dev libglew-dev
```

Windows users do not need to install dependencies

### Cloning the project

```
git clone git@github.com:emulated-gamer-bois/gameboy-emulator.git
cd gameboy-emulator
git submodule update --init --recursive
```
### Building and run the project: Command line

```
mkdir cmake-build-debug
cd cmake-build-debug
cmake ..
cmake --build .
src/project
```

### Build and run the project: CLion

- Open the project in CLion
- Select `Edit Configurations...`, set `Working Directory` to `$FileDir$` and press `OK`
- Rebuild and run the project

## Known issues
If cmake fails to find `SDL::SDL`, remove this line (or comment it out with `#`) from `labhelper/CMakeList.txt`
