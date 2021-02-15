cmake_minimum_required ( VERSION 3.0.2 )
project( gameboy )

add_executable( ${PROJECT_NAME} cpu.cpp cpu.h register_pair.h mmu.h mmu.cpp)
