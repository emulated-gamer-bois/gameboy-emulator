cmake_minimum_required ( VERSION 3.0.2 )
project( gameboy )

add_library( ${PROJECT_NAME} CPU.cpp CPU.h RegisterPair.h MMU.h MMU.cpp GameBoy.h GameBoy.cpp PPU.cpp PPU.h Sprite.cpp Sprite.h)
