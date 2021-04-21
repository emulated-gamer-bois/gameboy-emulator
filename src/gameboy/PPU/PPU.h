//
// Created by riddarvid on 2/23/21.
//

#pragma once

#include <memory> // smart pointers
#include <cstdint> // uint8_t and uint16_t
#include <array> // frame buffer
#include <queue>

#include "../Definitions.h" // LCD_WIDTH and LCD_HEIGHT
#include "../MMU/MMU.h"
#include "Sprite.h"

// Register addresses
#define LCDC_ADDRESS    0xFF40
#define STAT_ADDRESS    0xFF41
#define SCY_ADDRESS     0xFF42
#define SCX_ADDRESS     0xFF43
#define LY_ADDRESS      0xFF44
#define LYC_ADDRESS     0xFF45
#define DMA_ADDRESS     0xFF46
#define BGP_ADDRESS     0xFF47
#define OBP0_ADDRESS    0xFF48
#define OBP1_ADDRESS    0xFF49
#define WY_ADDRESS      0xFF4A
#define WX_ADDRESS      0xFF4B

// Tilemap memory block modes.
#define BG_WINDOW_MAP0  0x9800
#define BG_WINDOW_MAP1  0x9C00

// Tileset memory bock modes.
#define BG_WINDOW_TILE_DATA0 0x9000
#define BG_WINDOW_TILE_DATA1 0x8000

#define FRIEND_TEST(test_case_name, test_name)\
friend class test_case_name##_##test_name##_Test

class PPU {
public:
    explicit PPU(std::shared_ptr<MMU> mmu);

    //Device methods
    uint8_t read(uint16_t addr) const;
    void write(uint16_t addr, uint8_t data);

    //Drawing methods
    void reset();
    void update(uint16_t cpuCycles);
    bool isReadyToDraw() const;
    void confirmDraw();
    const std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT>* getFrameBuffer() const;
private:
    std::shared_ptr<MMU> memory;

    //The amount of cycles each mode should last
    const static uint16_t HBLANK_THRESHOLD = 51;
    const static uint16_t VBLANK_LINE_THRESHOLD = 114;
    const static uint16_t OAM_SEARCH_THRESHOLD = 20;
    const static uint16_t SCANLINE_DRAW_THRESHOLD = 43;
    uint16_t accumulatedCycles{};

    //The four modes of the PPU
    enum Mode {
        HBLANK,
        VBLANK,
        OAM_SEARCH,
        SCANLINE_DRAW
    };

    // LCD control register
    union {
        struct {
            unsigned int bgWindowDisplayEnable: 1;
            unsigned int objectDisplayEnable: 1;
            unsigned int objectSize : 1;
            unsigned int bgTileMapSelect : 1;
            unsigned int bgWindowTileSetSelect : 1;
            unsigned int windowDisplayEnable : 1;
            unsigned int windowTileMapSelect : 1;
            unsigned int lcdDisplayEnable : 1;
        };
        uint8_t LCDC{};
    };

    // LCD status register
    union {
        struct {
            unsigned int modeFlag: 2;
            unsigned int coincidenceFlag: 1;
            unsigned int hBlankInterruptEnable : 1;
            unsigned int vBlankInterruptEnable : 1;
            unsigned int oamInterruptEnable : 1;
            unsigned int lycEqualsLyInterruptEnable : 1;
            unsigned int UNUSED : 1;
        };
        uint8_t STAT{};
    };

    // Misc registers
    uint8_t SCY{};
    uint8_t SCX{};
    uint8_t LY{};
    uint8_t LYC{};
    uint8_t DMA{};
    uint8_t WY{};
    uint8_t WX{};
    uint8_t BGP{};
    uint8_t OBP0{};
    uint8_t OBP1{};

    //The color indexes of the background and window this scanline. Used to determine priority of sprites
    std::array<uint8_t, LCD_WIDTH> bgWindowColorIndexesThisLine{};
    std::priority_queue<Sprite> spritesNextScanLine;
    std::array<uint8_t, LCD_WIDTH * LCD_HEIGHT> frameBuffer{};

    bool readyToDraw{};
    bool anyStatConditionLastUpdate{};

    //Scanline methods
    void processNextLine();
    void drawBackgroundScanLine();
    void drawWindowScanLine();
    void drawObjectScanLine();

    //Sprite methods
    void loadSpritesNextScanLine();
    Sprite loadSprite(uint8_t index);
    uint8_t getSpritePixelColorIndex(Sprite & sprite, uint8_t lcdX, uint8_t lcdY);

    // Parsing methods
    uint8_t getTileID(uint16_t mapStart, uint8_t pixelAbsoluteX, uint8_t pixelAbsoluteY);
    uint8_t getTilePixelColorIndex(uint8_t tileSet, uint8_t id, uint8_t tileX, uint8_t tileY);
    static uint8_t getColor(uint8_t palette, uint8_t colorIndex);

    // Interrupt related methods
    void vBlankInterrupt();
    void statInterrupt();
    bool meetsStatConditions() const;

    //DMA transfer
    void dma_transfer(uint8_t startAddress);
};
