cmake_minimum_required ( VERSION 3.0.2 )
project( gameboy )

add_library( ${PROJECT_NAME}
        CPU.cpp
        CPU.h
        RegisterPair.h
        MMU.h
        MMU.cpp
        GameBoy.h
        GameBoy.cpp
        PPU/PPU.cpp
        PPU/PPU.h
        PPU/Sprite.cpp
        PPU/Sprite.h
        Timer.h
        Timer.cpp
        Joypad.h
        Joypad.cpp
        Cartridge.h
        Cartridge.cpp
        MBC.h
        MBC.cpp APU/APU.cpp APU/APU.h)
