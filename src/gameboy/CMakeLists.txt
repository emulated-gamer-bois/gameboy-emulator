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
        PPU.cpp
        PPU.h
        Timer.h
        Timer.cpp
        Joypad.h
        Joypad.cpp
        Cartridge.h
        Cartridge.cpp
        MBC.h
        MBC.cpp)
