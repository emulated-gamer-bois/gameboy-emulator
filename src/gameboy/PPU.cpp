//
// Created by riddarvid on 2/23/21.
//

#include "PPU.h"
#include "Definitions.h"
#include <cassert>

PPU::PPU(std::shared_ptr<MMU> memory) {
    SCY = 0;
    SCX = 0;
    LY = 0;
    LYC = 0;
    WY = 0;
    WX = 0;

    BGP = 0;
    OBP0 = 0;
    OBP1 = 0;

    DMA = 0;

    STAT = 0;
    LCDC = 0;

    this->memory = memory;
    this->accumulatedCycles = 0;
    this->modeFlag = OAM_SEARCH;
    saveRegisters();
    frameBuffer.fill(0);
    this->readyToDraw = false;
    this->anyStatConditionLastUpdate = false;
}

void PPU::update(uint16_t cpuCycles) {
    initRegisters();
    accumulatedCycles += cpuCycles;
    switch(modeFlag) { //depending on which mode the PPU is in
        case HBLANK:
            if (accumulatedCycles >= HBLANK_THRESHOLD) {
                accumulatedCycles -= HBLANK_THRESHOLD;
                LY++;

                if (LY < 144) { //If we still have lines to draw
                    modeFlag = OAM_SEARCH;
                } else { //If we have drawn the entire screen
                    modeFlag = VBLANK;
                    vBlankInterrupt();
                }
            }
            break;

        case VBLANK:
            if (accumulatedCycles >= VBLANK_LINE_THRESHOLD) {
                accumulatedCycles -= VBLANK_LINE_THRESHOLD;
                LY++;

                if (LY == 154) { //If the VBLANK should end: Reset LY and clear frame buffer
                    LY = 0;
                    //frameBuffer.fill(0);
                    modeFlag = OAM_SEARCH;
                    // If leaving VBLANK we are ready to draw the screen
                    readyToDraw = true;
                }
            }
            break;

        case OAM_SEARCH:
            if (LY == 0) {
                frameBuffer.fill(0);
            }
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
    bool meetsStatConditionsCurrent = meetsStatConditions();
    if (!anyStatConditionLastUpdate) {
        if (meetsStatConditionsCurrent) {
            statInterrupt();
        }
    }
    anyStatConditionLastUpdate = meetsStatConditionsCurrent;
    saveRegisters();
}

void PPU::processNextLine() {
    if (bgWindowDisplayEnable) {
        drawBackgroundScanLine();
    }
    //TODO window and object drawing
}

void PPU::initRegisters() {
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

void PPU::saveRegisters() { //TODO check if anything else needs doing here
    memory->write(LCDC_ADDRESS, LCDC);
    memory->write(STAT_ADDRESS, STAT);

    memory->write(LY_ADDRESS, LY);
}

void PPU::drawBackgroundScanLine() {
    uint16_t bgMapStartAddress;
    if (tileMapDisplaySelect) {
        bgMapStartAddress = BG_WINDOW_MAP1;
    } else {
        bgMapStartAddress = BG_WINDOW_MAP0;
    }

    //For each pixel in the current row, find the correct tile ID, then the
    //correct pixel in that tile
    for (uint8_t x = 0; x < 160; ++x) {
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
uint8_t PPU::getTileID(uint16_t bgMapStart, uint8_t pixelAbsoluteX, uint8_t pixelAbsoluteY) {
    uint16_t tileAbsoluteX = pixelAbsoluteX / 8; //Divide by 8 since the width and height of a tile is 8
    uint16_t tileAbsoluteY = pixelAbsoluteY / 8;
    uint16_t offset = tileAbsoluteY * 32 + tileAbsoluteX; //Convert from 2D matrix to array index
    return memory->read(bgMapStart + offset);
}

uint8_t PPU::getTilePixelColor(uint8_t tileID, uint8_t absolutePixelX, uint8_t absolutePixelY) { //TODO test
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
    auto tilePixelX = 7 - (absolutePixelX % 8);

    uint8_t lowByte = memory->read(address + tilePixelY * 2);
    uint8_t highByte = memory->read(address + tilePixelY * 2 + 1);

    uint8_t lowBit = (lowByte >> tilePixelX) & 1;
    uint8_t highBit = (highByte >> tilePixelX) & 1;

    uint8_t pixelColor = (highBit << 1) | lowBit;

    assert(pixelColor >= 0 && pixelColor <= 3); // If not true, there is a bug in the code. Temporary line?

    uint8_t bitmask = 3;
    return ((BGP >> (2 * pixelColor)) & bitmask);
}

std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT>* PPU::getFrameBuffer() {
    return &frameBuffer;
}

uint8_t PPU::getMode() const {
    return modeFlag;
}

bool PPU::meetsStatConditions() {
    if (lycEqualsLyInterruptEnable && LY == LYC) {
        coincidenceFlag = 1;
        return true;
    }
    if (oamInterruptEnable && modeFlag == OAM_SEARCH) {
        return true;
    }
    if (vBlankInterruptEnable && modeFlag == VBLANK) {
        return true;
    }
    if (hBlankInterruptEnable && modeFlag == HBLANK) {
        return true;
    }
    return false;
}

void PPU::vBlankInterrupt() {
    uint8_t interruptFlags = memory->read(INTERRUPT_FLAG);
    interruptFlags |= (1 << 0);
    memory->write(INTERRUPT_FLAG, interruptFlags);
}

void PPU::statInterrupt() {
    uint8_t interruptFlags = memory->read(INTERRUPT_FLAG);
    interruptFlags |= (1 << 1);
    memory->write(INTERRUPT_FLAG, interruptFlags);
}

bool PPU::isReadyToDraw() const {
    return this->readyToDraw;
}

void PPU::confirmDraw() {
    this->readyToDraw = false;
}
