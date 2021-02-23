//
// Created by riddarvid on 2/23/21.
//

#include "ppu.h"

#define LCDC_ADDRESS 0xFF40
#define STAT_ADDRESS 0xFF41
#define SCY_ADDRESS 0xFF42
#define SCX_ADDRESS 0xFF43
#define LY_ADDRESS 0xFF44
#define LYC_ADDRESS 0xFF45
#define DMA_ADDRESS 0xFF46
#define BGP_ADDRESS 0xFF47
#define OBP0_ADDRESS 0xFF48
#define OBP1_ADDRESS 0xFF49
#define WY_ADDRESS 0xFF4A
#define WX_ADDRESS 0xFF4B

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
    initProcessNextLine();
    //TODO actual "drawing" code
    endProcessNextLine();
}

void ppu::initProcessNextLine() {
    LCDC = memory->read(LCDC_ADDRESS);
    STAT = memory->read(STAT_ADDRESS);
    
    SCY = memory->read(SCY_ADDRESS);
    SCX = memory->read(SCX_ADDRESS);
    LY = memory->read(LY_ADDRESS);
    LYC = memory->read(LYC_ADDRESS);
    WY = memory->read(WY_ADDRESS);
    WX = memory->read(WX_ADDRESS);

    BGP = memory->read(BGP_ADDRESS);
    OBP0 = memory->read(OBP0_ADDRESS);
    OBP1 = memory->read(OBP1_ADDRESS);
    DMA = memory->read(DMA_ADDRESS);
}

void ppu::endProcessNextLine() { //TODO check if anything else needs doing here
    memory->write(LCDC_ADDRESS, LCDC);
    memory->write(STAT_ADDRESS, STAT);

    memory->write(LY_ADDRESS, (LY + 1) % LCD_HEIGHT);
}
