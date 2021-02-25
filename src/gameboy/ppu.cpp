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
    this->cyclesSinceLastLine = 0;
}

void ppu::update(uint16_t cpuCycles) {
    cyclesSinceLastLine += cpuCycles;
    while (cyclesSinceLastLine >= THRESHOLD) {
        cyclesSinceLastLine -= THRESHOLD;
        processNextLine();
    }
}

void ppu::processNextLine() { //TODO fix state machine so that the method is only ran when not in a vblank
    initProcessNextLine();
    drawBackgroundScanLine();
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

    memory->write(LY_ADDRESS, (LY + 1) % LCD_HEIGHT); //Increments LY, since we are done processing the current line
}

void ppu::drawBackgroundScanLine() {
    uint16_t bgMapStart;
    if (tileMapDisplaySelect) {
        bgMapStart = BG_WINDOW_MAP1;
    } else {
        bgMapStart = BG_WINDOW_MAP0;
    }

    for (uint8_t x = 0; x < 160; ++x) {
        uint8_t absolutePixelX = (SCX + x) % 256;
        uint8_t absolutePixelY = (SCY + LY) % 256;
        uint8_t tileID = getTileID(bgMapStart, absolutePixelX, absolutePixelY);
        uint8_t pixel = getTilePixelColor(tileID, absolutePixelX, absolutePixelY);
        bytes[LY * 160 + x] = pixel;
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
    uint16_t tileAbsoluteX = pixelAbsoluteX / 8;
    uint16_t tileAbsoluteY = pixelAbsoluteY / 8;
    uint16_t offset = tileAbsoluteY * 32 + tileAbsoluteX;
    return memory->read(bgMapStart + offset);
}

uint8_t ppu::getTilePixelColor(uint8_t tileID, uint8_t absolutePixelX, uint8_t absolutePixelY) { //TODO test
    uint16_t tileDataBasePointer;
    uint16_t address;
    if (bgWindowTileDataSelect) {
        tileDataBasePointer = BG_WINDOW_TILE_DATA1;
        address = tileID * 16 + tileDataBasePointer;
    } else {
        tileDataBasePointer = BG_WINDOW_TILE_DATA0;
        auto signedID = (int8_t)tileID;
        address = signedID * 16 + tileDataBasePointer;
    }

    uint8_t tileY = absolutePixelY % 8;
    uint8_t tileX = 7 - (absolutePixelX % 8);

    uint8_t lowByte = memory->read(address + tileY * 2);
    uint8_t highByte = memory->read(address + tileY * 2 + 1);

    uint8_t lowBit = lowByte & (1 << tileX);
    uint8_t highBit = highByte & (1 << tileX);

    uint8_t pixelColor = (highBit << 1) | lowBit;

    assert(pixelColor >= 0 && pixelColor <= 3); // If not true, there is a bug in the code. Temporary line?

    auto bitmask = 3;
    return BGP & (bitmask << pixelColor * 2);
}

std::array<uint8_t, 160 * 144> ppu::getBytes() { //TODO fix dimensions
    return bytes;
}
