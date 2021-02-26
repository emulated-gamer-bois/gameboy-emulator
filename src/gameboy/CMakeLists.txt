cmake_minimum_required ( VERSION 3.0.2 )
project( gameboy )

add_library( ${PROJECT_NAME} CPU.cpp CPU.h register_pair.h MMU.h MMU.cpp gameboy.h gameboy.cpp PPU.cpp PPU.h)
