//
// Created by riddarvid on 2/23/21.
//

#include "PPU.h"
#include <memory>
#include <iostream>

PPU::PPU(std::shared_ptr<MMU> memory) {
    this->memory = memory;
    this->reset();
}

uint8_t PPU::read(uint16_t addr) const {
    switch (addr) {
        case LCDC_ADDRESS:
            return this->LCDC;
        case STAT_ADDRESS:
            return this->STAT;
        case SCY_ADDRESS:
            return this->SCY;
        case SCX_ADDRESS:
            return this->SCX;
        case LY_ADDRESS:
            return this->LY;
        case LYC_ADDRESS:
            return this->LYC;
        case DMA_ADDRESS:
            return this->DMA;
        case BGP_ADDRESS:
            return this->BGP;
        case OBP0_ADDRESS:
            return this->OBP0;
        case OBP1_ADDRESS:
            return this->OBP1;
        case WY_ADDRESS:
            return this->WY;
        case WX_ADDRESS:
            return this->WX;
        default:
            return 0;
    }
}

void PPU::write(uint16_t addr, uint8_t data) {
    switch (addr) {
        case LCDC_ADDRESS:
            this->LCDC = data;
            break;
        case STAT_ADDRESS:
            statInterrupt(); //Hardware bug, interrupt should be thrown every time STAT is written
            this->STAT = data;
            break;
        case SCY_ADDRESS:
            this->SCY = data;
            break;
        case SCX_ADDRESS:
            this->SCX = data;
            break;
        case LY_ADDRESS:
            break;
        case LYC_ADDRESS:
            this->LYC = data;
            break;
        case DMA_ADDRESS:
            this->dma_transfer(data);
            break;
        case BGP_ADDRESS:
            this->BGP = data;
            break;
        case OBP0_ADDRESS:
            this->OBP0 = data;
            break;
        case OBP1_ADDRESS:
            this->OBP1 = data;
            break;
        case WY_ADDRESS:
            this->WY = data;
            break;
        case WX_ADDRESS:
            this->WX = data;
            break;
        default:
            std::cout << "Tried to write data: " << (int)data << " to address: " << (int)addr << std::endl;
    }
}

void PPU::reset() {
    this->SCY = 0;
    this->SCX = 0;
    this->LY = 0;
    this->LYC = 0;
    this->DMA = 0;
    this->WY = 0;
    this->WX = 0;
    this->BGP = 0;
    this->OBP0 = 0;
    this->OBP1 = 0;
    this->STAT = 0;
    this->LCDC = 0;

    this->accumulatedCycles = 0;
    this->modeFlag = OAM_SEARCH;
    this->readyToDraw = false;
    this->anyStatConditionLastUpdate = false;

    frameBuffer.fill(0);
    bgWindowColorIndexesThisLine.fill(0);
}

void PPU::update(uint16_t cpuCycles) {
    accumulatedCycles += cpuCycles;
    switch(modeFlag) {
        case HBLANK:
            //When the h-blank phase ends, LY is increased. Depending on LY, we either go to OAM search or v-blank.
            // If we go to v-blank, we throw a v-blank interrupt.
            if (accumulatedCycles >= HBLANK_THRESHOLD) {
                accumulatedCycles -= HBLANK_THRESHOLD;
                LY++;

                if (LY < LCD_HEIGHT) { //If we still have lines to draw
                    modeFlag = OAM_SEARCH;
                } else { //If we have drawn the entire screen
                    modeFlag = VBLANK;
                    vBlankInterrupt();
                }
            }
            break;

        case VBLANK:
            //When the v-blank phase ends, LY is increased.
            // If the entire frame has been drawn, LY is reset and the PPU goes to the OAM search phase.
            if (accumulatedCycles >= VBLANK_LINE_THRESHOLD) {
                accumulatedCycles -= VBLANK_LINE_THRESHOLD;
                LY++;

                if (LY == LCD_HEIGHT + 10) {
                    LY = 0;
                    modeFlag = OAM_SEARCH;
                    // If leaving VBLANK we are ready to draw the screen.
                    readyToDraw = true;
                }
            }
            break;

        case OAM_SEARCH:
            //When the OAM search phase ends, the coincidence flag is updated, the sprites for the scanline are loaded and the PPU goes to the draw phase.
            if (accumulatedCycles >= OAM_SEARCH_THRESHOLD) {
                accumulatedCycles -= OAM_SEARCH_THRESHOLD;
                coincidenceFlag = (LYC == LY); // Set coincidence flag before drawing the scanline.
                loadSpritesNextScanLine();
                modeFlag = SCANLINE_DRAW;
            }
            break;

        case SCANLINE_DRAW:
            //When the draw phase ends, the current line is processed and the ppu goes into the h-blank phase.
            if (accumulatedCycles >= SCANLINE_DRAW_THRESHOLD) {
                accumulatedCycles -= SCANLINE_DRAW_THRESHOLD;
                processNextLine();
                modeFlag = HBLANK;
            }
            break;
    }

    //A STAT-interrupt should be thrown when going from no conditions met to any conditions met.
    bool meetsStatConditionsCurrent = meetsStatConditions();
    if (!anyStatConditionLastUpdate) {
        if (meetsStatConditionsCurrent) {
            statInterrupt();
        }
    }
    anyStatConditionLastUpdate = meetsStatConditionsCurrent;
}

bool PPU::isReadyToDraw() const {
    return this->readyToDraw;
}

void PPU::confirmDraw() {
    //TODO Filling the frame buffer with zeroes seems unnecessary?
   // this->frameBuffer.fill(0);
    this->readyToDraw = false;
}

const std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT>* PPU::getFrameBuffer() const {
    return &frameBuffer;
}

void PPU::processNextLine() {
    if (lcdDisplayEnable) {
        if (bgWindowDisplayEnable) {
            drawBackgroundScanLine();
            if (windowDisplayEnable) {
                drawWindowScanLine();
            }
        }
        if (objectDisplayEnable) {
            drawObjectScanLine();
        }
    }
}

void PPU::drawBackgroundScanLine() {
    uint16_t bgMapStartAddress;
    if (bgTileMapSelect) {
        bgMapStartAddress = BG_WINDOW_MAP1;
    } else {
        bgMapStartAddress = BG_WINDOW_MAP0;
    }

    for (uint8_t x = 0; x < LCD_WIDTH; ++x) {
        uint8_t absolutePixelX = (SCX + x) % BACKGROUND_WIDTH;
        uint8_t absolutePixelY = (SCY + LY) % BACKGROUND_HEIGHT;
        uint8_t tileID = getTileID(bgMapStartAddress, absolutePixelX, absolutePixelY);
        uint8_t colorIndex = getTilePixelColorIndex(bgWindowTileSetSelect, tileID, absolutePixelX % 8, absolutePixelY % 8);
        uint8_t pixel = getColor(BGP, colorIndex);
        frameBuffer[LY * LCD_WIDTH + x] = pixel;
        bgWindowColorIndexesThisLine[x] = pixel;
    }
}

void PPU::drawWindowScanLine() {
    if (WY > LY) {
        return;
    }
    uint16_t windowMapStartAddress;
    if (windowTileMapSelect) {
        windowMapStartAddress = BG_WINDOW_MAP1;
    } else {
        windowMapStartAddress = BG_WINDOW_MAP0;
    }
    int startX = WX - 7; //WX = windows start position + 7
    if (startX < 0) {
        startX = 0;
    }
    for (int x = startX; x < LCD_WIDTH; ++x) {
        uint8_t absolutePixelX = x - startX; //TODO check hardware bug when 0 < WX <= 6 and WX = 166 What is the intended behaviour?
        uint8_t absolutePixelY = LY - WY;
        uint8_t tileID = getTileID(windowMapStartAddress, absolutePixelX, absolutePixelY);
        uint8_t colorIndex = getTilePixelColorIndex(bgWindowTileSetSelect, tileID, absolutePixelX % 8, absolutePixelY % 8);
        uint8_t pixel = getColor(BGP, colorIndex);
        frameBuffer[LY * LCD_WIDTH + x] = pixel;
        bgWindowColorIndexesThisLine[x] = pixel;
    }
}

void PPU::drawObjectScanLine() {
    while (!spritesNextScanLine.empty()) {
        auto sprite = spritesNextScanLine.top();
        spritesNextScanLine.pop();
        for (int x = sprite.getX(); x < sprite.getX() + 8; ++x) {
            if (x < 0 || x >= 160) {
                continue;
            }
            //If the sprite should be behind the background and the background is not color 0, don't display the pixel
            if ((sprite.backgroundOverSprite()) && (bgWindowColorIndexesThisLine[x] != 0)) { //TODO should this be color index 0 or color 0?
                frameBuffer[LY * LCD_WIDTH + x] = getColor(BGP, bgWindowColorIndexesThisLine[x]);
            } else {
                uint8_t colorIndex = getSpritePixelColorIndex(sprite, x, LY);
                if (colorIndex != 0) { //Color index 0 is transparent
                    uint8_t pixel;
                    if (sprite.getPaletteNumber()) {
                        pixel = getColor(OBP1, colorIndex);
                    } else {
                        pixel = getColor(OBP0, colorIndex);
                    }
                    frameBuffer[LY * LCD_WIDTH + x] = pixel;
                }
            }
        }
    }
}

void PPU::loadSpritesNextScanLine() {
    //Loads the ten first sprites in OAM that appear on the current scanline.
    //There are a maximum of 40 sprites total and 10 sprites per scanline.
    for (int i = 0; i < 40 && spritesNextScanLine.size() < 10; ++i) {
        Sprite sprite = loadSprite(i);
        if (sprite.coversLine(LY, objectSize)) {
            spritesNextScanLine.push(sprite);
        }
    }
}

Sprite PPU::loadSprite(uint8_t index) {
    uint16_t startAddress = OAM_START + index * 4; //Each sprite occupies four bytes
    uint8_t yByte = memory->read(startAddress);
    uint8_t xByte = memory->read(startAddress + 1);
    uint8_t tileIndex = memory->read(startAddress + 2);
    uint8_t flags = memory->read(startAddress + 3);
    return {yByte, xByte, tileIndex, flags, index};
}


uint8_t PPU::getSpritePixelColorIndex(Sprite & sprite, uint8_t lcdX, uint8_t lcdY) {
    uint8_t tileID = sprite.getTileID(lcdY, objectSize);
    uint8_t tileX = sprite.getTileX(lcdX);
    uint8_t tileY = sprite.getTileY(lcdY);
    return getTilePixelColorIndex(1, tileID, tileX, tileY); //Sprites always use tile set 1
}

uint8_t PPU::getTileID(uint16_t mapStart, uint8_t pixelAbsoluteX, uint8_t pixelAbsoluteY) {
    //Divide by x and y by 8, since the width and height of a tile is 8
    uint16_t tileAbsoluteX = pixelAbsoluteX / 8;
    uint16_t tileAbsoluteY = pixelAbsoluteY / 8;
    uint16_t offset = tileAbsoluteY * 32 + tileAbsoluteX; //Convert from 2D matrix to array index
    return memory->read(mapStart + offset);
}

uint8_t PPU::getTilePixelColorIndex(uint8_t tileSet, uint8_t tileId, uint8_t tileX, uint8_t tileY) {
    uint16_t address;

    //Find the address of the tile with id tileID, depending on addressing mode
    if (tileSet) {
        address = tileId * 16 + BG_WINDOW_TILE_DATA1;
    } else {
        auto signedID = (int8_t)tileId;
        address = signedID * 16 + BG_WINDOW_TILE_DATA0;
    }

    //Pixels in a row are numbered 7 to 0
    tileX = 7 - tileX;

    //Read the two bytes associated with the correct row of the tile
    uint8_t lowByte = memory->read(address + tileY * 2);
    uint8_t highByte = memory->read(address + tileY * 2 + 1);

    //Determine the two bits associated with the correct pixel in the row
    uint8_t lowBit = (lowByte >> tileX) & 1;
    uint8_t highBit = (highByte >> tileX) & 1;

    //Combine the two bits to form the color index of the pixel
    uint8_t pixelColor = (highBit << 1) | lowBit;
    return pixelColor;
}

uint8_t PPU::getColor(uint8_t palette, uint8_t colorIndex) {
    uint8_t bitmask = 0b11;
    return ((palette >> 2 * colorIndex) & bitmask);
}

void PPU::vBlankInterrupt() {
    memory->raise_interrupt_flag(V_BLANK_IF_BIT);
}

void PPU::statInterrupt() {
    memory->raise_interrupt_flag(STAT_IF_BIT);
}

bool PPU::meetsStatConditions() const {
    auto lycInterrupt = (lycEqualsLyInterruptEnable && coincidenceFlag);
    auto oamInterrupt = (oamInterruptEnable && (modeFlag == OAM_SEARCH));
    auto vBlankInterrupt = (vBlankInterruptEnable && (modeFlag == VBLANK));
    auto hBlankInterrupt = (hBlankInterruptEnable && (modeFlag == HBLANK));

    return (lycInterrupt || oamInterrupt || vBlankInterrupt || hBlankInterrupt);
}

void PPU::dma_transfer(uint8_t startAddress) {
    if (0x00 <= startAddress && startAddress <= 0xdf) {
        uint16_t start_addr = (startAddress << 8);
        for (uint8_t i = 0; i <= 0x9f; i++) {
            this->memory->write(OAM_START + i, this->memory->read(start_addr+i));
        }
    } else {
        std::cout << "Tried to use DMA transfer with invalid input: " << startAddress << std::endl;
    }
}
