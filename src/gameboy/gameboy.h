//
// Created by algot on 2021-02-23.
//

#ifndef LAME_BOY_SRC_GAMEBOY_GAMEBOY_H_
#define LAME_BOY_SRC_GAMEBOY_GAMEBOY_H_

#include <memory>
#include "cpu.h"
#include "mmu.h"

#define JOYPAD_RELEASE 0
#define JOYPAD_PRESS   1

class GameBoy {
public:
    GameBoy();
    void step();
    uint8_t *getScreen();
    void joypad_input(uint8_t key, uint8_t action);

private:
    std::shared_ptr<MMU> mmu;
    std::unique_ptr<CPU> cpu;

    // std::unique_ptr<GPU> ...... TODO: Wait for ppu implementation
};

#endif //LAME_BOY_SRC_GAMEBOY_GAMEBOY_H_
