//
// Created by riddarvid on 2/23/21.
//

#pragma once

#include <memory> // smart pointers
#include <cstdint> // uint8_t and uint16_t
#include "MMU.h"
#include "Definitions.h" // LCD_WIDTH and LCD_HEIGHT

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

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

class PPU {
public:
    explicit PPU(std::shared_ptr<MMU> memory);
    void update(uint16_t cpuCycles);

    enum Mode {
        HBLANK,
        VBLANK,
        OAM_SEARCH,
        SCANLINE_DRAW
    };

    bool getReadyToDraw() const;
    std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT>* getFrameBuffer();
    uint8_t getMode() const;
private:
    const static uint16_t HBLANK_THRESHOLD = 51; //The time it takes to search for objects, draw the line and HBLANK.
    const static uint16_t VBLANK_LINE_THRESHOLD = 114;
    const static uint16_t OAM_SEARCH_THRESHOLD = 20;
    const static uint16_t SCANLINE_DRAW_THRESHOLD = 43;

    std::shared_ptr<MMU> memory;
    std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT> frameBuffer;
    uint16_t accumulatedCycles;

    union {
        struct { //bits 0-7
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
    void initRegisters();
    void drawBackgroundScanLine();
    void saveRegisters();
    uint8_t getTileID(uint16_t bgMapStart, uint8_t pixelAbsoluteX, uint8_t pixelAbsoluteY);
    uint8_t getTilePixelColor(uint8_t id, uint8_t x, uint8_t y);

    bool readyToDraw;
};
