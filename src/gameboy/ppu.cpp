//
// Created by riddarvid on 2/23/21.
//

#include "ppu.h"
#include <cassert>

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

#define BG_WINDOW_MAP0 0x9800
#define BG_WINDOW_MAP1 0x9C00

#define BG_WINDOW_TILE_DATA0 0x9000
#define BG_WINDOW_TILE_DATA1 0x8000

ppu::ppu(std::shared_ptr<MMU> memory) {
    this->memory = memory;
    this->accumulatedCycles = 0;
    this->modeFlag = OAM_SEARCH;
    bytes.fill(0);
}

void ppu::update(uint16_t cpuCycles) {
    initRegisters();
    accumulatedCycles += cpuCycles;
    switch(modeFlag) { //depending on which mode the ppu is in
        case HBLANK:
            if (accumulatedCycles >= HBLANK_THRESHOLD) {
                accumulatedCycles -= HBLANK_THRESHOLD;
                LY++;

                if (LY < 144) { //If we still have lines to draw
                    modeFlag = OAM_SEARCH;
                } else { //If we have drawn the entire screen
                    modeFlag = VBLANK;
                }
            }
            break;

        case VBLANK:
            if (accumulatedCycles >= VBLANK_LINE_THRESHOLD) {
                accumulatedCycles -= VBLANK_LINE_THRESHOLD;
                LY++;

                if (LY == 154) { //If the VBLANK should end: Reset LY and clear frame buffer
                    LY = 0;
                    bytes.fill(0);
                    modeFlag = OAM_SEARCH;
                }
            }
            break;

        case OAM_SEARCH:
            if (accumulatedCycles >= OAM_SEARCH_THRESHOLD) {
                accumulatedCycles -= OAM_SEARCH_THRESHOLD;
                modeFlag = SCANLINE_DRAW;
            }
            break;

        case SCANLINE_DRAW:
            if (accumulatedCycles >= SCANLINE_DRAW_THRESHOLD) {
                accumulatedCycles -= SCANLINE_DRAW_THRESHOLD;
                processNextLine();
                modeFlag = HBLANK;
            }
            break;
    }
    saveRegisters();
}

void ppu::processNextLine() {
    if (bgWindowDisplayEnable) {
        drawBackgroundScanLine();
    }
    //TODO window and object drawing
}

void ppu::initRegisters() {
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

void ppu::saveRegisters() { //TODO check if anything else needs doing here
    memory->write(LCDC_ADDRESS, LCDC);
    memory->write(STAT_ADDRESS, STAT);

    memory->write(LY_ADDRESS, LY);
}

void ppu::drawBackgroundScanLine() {
    uint16_t bgMapStartAddress;
    if (tileMapDisplaySelect) {
        bgMapStartAddress = BG_WINDOW_MAP1;
    } else {
        bgMapStartAddress = BG_WINDOW_MAP0;
    }

    for (uint8_t x = 0; x < 160; ++x) { //For each pixel in the current row, find the correct tile ID, then the
                                        //correct pixel in that tile
        uint8_t absolutePixelX = (SCX + x) % 256;
        uint8_t absolutePixelY = (SCY + LY) % 256;
        uint8_t tileID = getTileID(bgMapStartAddress, absolutePixelX, absolutePixelY);
        uint8_t pixel = getTilePixelColor(tileID, absolutePixelX, absolutePixelY);
        frameBuffer[LY * 160 + x] = pixel;
    }
}

/**
 *
 * @param bgMapStart
 * @param pixelAbsoluteX
 * @param pixelAbsoluteY
 * @return The ID of the tile containing the target pixel
 */
uint8_t ppu::getTileID(uint16_t bgMapStart, uint8_t pixelAbsoluteX, uint8_t pixelAbsoluteY) {
    uint16_t tileAbsoluteX = pixelAbsoluteX / 8; //Divide by 8 since the width and height of a tile is 8
    uint16_t tileAbsoluteY = pixelAbsoluteY / 8;
    uint16_t offset = tileAbsoluteY * 32 + tileAbsoluteX; //Convert from 2D matrix to array index
    return memory->read(bgMapStart + offset);
}

uint8_t ppu::getTilePixelColor(uint8_t tileID, uint8_t absolutePixelX, uint8_t absolutePixelY) { //TODO test
    uint16_t startAddress;
    uint16_t address;

    if (bgWindowTileDataSelect) { //Find the address of the tile with id tileID, depending on addressing mode
        startAddress = BG_WINDOW_TILE_DATA1;
        address = tileID * 16 + startAddress;
    } else {
        startAddress = BG_WINDOW_TILE_DATA0;
        auto signedID = (int8_t)tileID;
        address = signedID * 16 + startAddress;
    }

    auto tilePixelY = absolutePixelY % 8;
    auto tilePixelX = absolutePixelX % 8;

    auto lowByte = memory->read(address + tilePixelY * 2);
    auto highByte = memory->read(address + tilePixelY * 2 + 1);

    auto lowBit = (lowByte >> tilePixelX) & 1;
    auto highBit = (highByte >> tilePixelX) & 1;

    auto pixelColor = (highBit << 1) | lowBit;

    assert(pixelColor >= 0 && pixelColor <= 3); // If not true, there is a bug in the code. Temporary line?

    auto bitmask = 3;
    return BGP & (bitmask << pixelColor * 2);
}

std::array<uint8_t, ppu::LCD_WIDTH * ppu::LCD_HEIGHT> ppu::getFrameBuffer() {
    return frameBuffer;
}