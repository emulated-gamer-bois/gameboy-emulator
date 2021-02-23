//
// Created by riddarvid on 2/23/21.
//

#include "ppu.h"

ppu::ppu(std::shared_ptr<MMU> memory) {
    this->memory = memory;
}
