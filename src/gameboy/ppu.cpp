//
// Created by riddarvid on 2/23/21.
//

#include "ppu.h"

#define LCDC_ADDRESS 0xFF40
#define STAT_ADDRESS 0xFF41
#define LY_ADDRESS 0xFF44


ppu::ppu(std::shared_ptr<MMU> memory) {
    this->memory = memory;
    this->cyclesSinceLastLine = 0;
}

void ppu::update(uint16_t cpuCycles) {
    cyclesSinceLastLine += cpuCycles;
    while (cyclesSinceLastLine >= THRESHOLD) {
        cyclesSinceLastLine -= THRESHOLD;
        processNextLine();
    }
}

void ppu::processNextLine() {
    uint8_t currentScanline = memory->read(LY_ADDRESS);
    LCDC = memory->read(LCDC_ADDRESS);
    STAT = memory->read(STAT_ADDRESS);


    memory->write(STAT_ADDRESS, STAT);
    memory->write(LCDC_ADDRESS, LCDC);
    currentScanline = (currentScanline + 1) % LCD_HEIGHT;
    memory->write(LY_ADDRESS, currentScanline);
}
