//
// Created by riddarvid on 2/23/21.
//

#ifndef LAME_BOY_PPU_H
#define LAME_BOY_PPU_H

#include <memory>
#include <cstdint>
#include "mmu.h"

class ppu {
    const static uint16_t THRESHOLD = 114; //The time it takes to search for objects, draw the line and HBLANK.
    const static uint16_t LCD_WIDTH = 160;
    const static uint16_t LCD_HEIGHT = 144;

    std::shared_ptr<MMU> memory;
    std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT> bytes;
    uint16_t cyclesSinceLastLine;

    enum Color {
        BLACK = 0x00,
        DARK_GREY = 0x68,
        LIGHT_GREY = 0xB8,
        WHITE = 0xFF
    };

    union {
        struct {
            unsigned int bgWindowDisplayEnable: 1;
            unsigned int objectDisplayEnable: 1;
            unsigned int objectSize : 1;
            unsigned int tileMapDisplaySelect : 1;
            unsigned int bgWindowTileDataSelect : 1;
            unsigned int windowDisplayEnable : 1;
            unsigned int windowTileMapDisplaySelect : 1;
            unsigned int lcdDisplayEnable : 1;
        };
        uint8_t LCDC;
    };

    union {
        struct {
            unsigned int modeFlag: 2;
            unsigned int coincidenceFlag: 1;
            unsigned int hBlankInterruptEnable : 1;
            unsigned int vBlankInterruptEnable : 1;
            unsigned int oamBlankInterruptEnable : 1;
            unsigned int lycEqualsLyInterruptEnable : 1;
            unsigned int UNUSED : 1;
        };
        uint8_t STAT;
    };

    uint8_t SCY;
    uint8_t SCX;
    uint8_t LY;
    uint8_t LYC;
    uint8_t WY;
    uint8_t WX;

    uint8_t BGP;
    uint8_t OBP0;
    uint8_t OBP1;

    uint8_t DMA;

    void processNextLine();
    void initProcessNextLine();
    void endProcessNextLine();
public:
    explicit ppu(std::shared_ptr<MMU> memory);
    void update(uint16_t cpuCycles);


};


#endif //LAME_BOY_PPU_H