cmake_minimum_required ( VERSION 3.0.2 )
project( gameboy )

add_library( ${PROJECT_NAME}
        CPU/CPU.cpp
        CPU/CPU.h
        CPU/RegisterPair.h
        MMU/MMU.h
        MMU/MMU.cpp
        GameBoy.h
        GameBoy.cpp
        PPU/PPU.cpp
        PPU/PPU.h
        PPU/Sprite.cpp
        PPU/Sprite.h
        MMU/Timer.h
        MMU/Timer.cpp
        Joypad.h
        Joypad.cpp
        MMU/Cartridge.h
        MMU/Cartridge.cpp
        MMU/MBC.h
        MMU/MBC.cpp APU/APU.cpp APU/APU.h APU/APUState.h
        )
