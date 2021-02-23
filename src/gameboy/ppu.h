//
// Created by riddarvid on 2/23/21.
//

#ifndef LAME_BOY_PPU_H
#define LAME_BOY_PPU_H

#include <memory>
#include "mmu.h"

class ppu {
    std::shared_ptr<MMU> memory;

public:
    ppu(std::shared_ptr<MMU> memory);
};


#endif //LAME_BOY_PPU_H
