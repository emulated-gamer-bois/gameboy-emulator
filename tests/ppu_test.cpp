//
// Created by riddarvid on 2/26/21.
//

#include <iostream>
#include <gtest/gtest.h>
#include "../src/gameboy/PPU.h"
#include <array>
#include <cstring>

std::array<char, 64> getWhiteTile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getGTile() {
    char tileData[8][8] = {
            {'W', 'W', 'B', 'B', 'B', 'W', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'L', 'W', 'W'},
            {'B', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'B', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'B', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'B', 'W', 'W', 'D', 'D', 'D', 'D', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'W', 'D', 'W'},
            {'W', 'W', 'B', 'B', 'B', 'W', 'D', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getATile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'D', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'D', 'W', 'D', 'W', 'W'},
            {'W', 'W', 'D', 'W', 'W', 'W', 'D', 'W'},
            {'W', 'W', 'D', 'D', 'D', 'D', 'D', 'W'},
            {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D'},
            {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D'},
            {'W', 'D', 'W', 'W', 'W', 'W', 'W', 'D'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getMTile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'L', 'W', 'L', 'W', 'W', 'W'},
            {'W', 'L', 'W', 'L', 'W', 'L', 'W', 'W'},
            {'W', 'L', 'W', 'L', 'W', 'L', 'W', 'W'},
            {'L', 'W', 'W', 'L', 'W', 'W', 'L', 'W'},
            {'L', 'W', 'W', 'W', 'W', 'W', 'L', 'W'},
            {'L', 'W', 'W', 'W', 'W', 'W', 'L', 'W'},
            {'L', 'W', 'W', 'W', 'W', 'W', 'L', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getETile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'B', 'B', 'B', 'B', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'B', 'B', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'B', 'B', 'B', 'B', 'W', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getBTile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'D', 'D', 'D', 'W', 'W', 'W', 'W'},
            {'W', 'D', 'W', 'W', 'D', 'W', 'W', 'W'},
            {'W', 'D', 'W', 'W', 'D', 'W', 'W', 'W'},
            {'W', 'D', 'W', 'D', 'W', 'W', 'W', 'W'},
            {'W', 'D', 'W', 'W', 'D', 'W', 'W', 'W'},
            {'W', 'D', 'W', 'W', 'D', 'W', 'W', 'W'},
            {'W', 'D', 'D', 'D', 'W', 'W', 'W', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getOTile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'L', 'L', 'L', 'W', 'W'},
            {'W', 'W', 'L', 'W', 'W', 'W', 'L', 'W'},
            {'W', 'L', 'W', 'W', 'W', 'W', 'W', 'L'},
            {'W', 'L', 'W', 'W', 'W', 'W', 'W', 'L'},
            {'W', 'L', 'W', 'W', 'W', 'W', 'W', 'L'},
            {'W', 'W', 'L', 'W', 'W', 'W', 'L', 'W'},
            {'W', 'W', 'W', 'L', 'L', 'L', 'W', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getYTile() {
    char tileData[8][8] = {
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'B', 'W', 'W'},
            {'W', 'B', 'W', 'W', 'W', 'B', 'W', 'W'},
            {'W', 'W', 'B', 'W', 'B', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'B', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'B', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'B', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'B', 'W', 'W', 'W', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

std::array<char, 64> getGScrolledXOnceTile() {
    char tileData[8][8] = {
            {'W', 'B', 'B', 'B', 'W', 'W', 'W', 'W'},
            {'B', 'W', 'W', 'W', 'L', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'W', 'W', 'W', 'W', 'W', 'W'},
            {'W', 'W', 'D', 'D', 'D', 'D', 'W', 'W'},
            {'B', 'W', 'W', 'W', 'W', 'D', 'W', 'W'},
            {'W', 'B', 'B', 'B', 'W', 'D', 'W', 'W'}
    };
    std::array<char, 64> tile{};
    std::memcpy(tile.data(), &tileData, 64);
    return tile;
}

void fillBackground(std::unique_ptr<PPU> & ppu, uint8_t tileMap, uint8_t tileID) {
    for (int i = 0; i < 32 * 32; ++i) {
        ppu->loadMapTESTING_ONLY(tileMap, i, tileID);
    }
}

void printTile00(std::unique_ptr<PPU> & ppu) {
    auto frameBuffer = ppu->getFrameBuffer();
    std::string reset     = "\033[0m";
    std::string white     = "\033[1;107m";
    std::string lightGrey = "\033[1;47m";
    std::string darkGrey  = "\033[1;100m";
    std::string black     = "\033[1;40m";

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            uint8_t color = frameBuffer->at(y * 160 + x);
            switch (color) {
                case 0:
                    printf("%s %s", white.data(), reset.data());
                    break;
                case 1:
                    printf("%s %s", lightGrey.data(), reset.data());
                    break;
                case 2:
                    printf("%s %s", darkGrey.data(), reset.data());
                    break;
                case 3:
                    printf("%s %s", black.data(), reset.data());
                    break;
                default:
                    exit(1);
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

void printScreen(std::unique_ptr<PPU> & ppu) {
    auto frameBuffer = ppu->getFrameBuffer();
    std::string reset     = "\033[0m";
    std::string white     = "\033[1;107m";
    std::string lightGrey = "\033[1;47m";
    std::string darkGrey  = "\033[1;100m";
    std::string black     = "\033[1;40m";

    for (int y = 0; y < 144; ++y) {
        for (int x = 0; x < 160; ++x) {
            uint8_t color = frameBuffer->at(y * 160 + x);
            switch (color) {
                case 0:
                    printf("%s %s", white.data(), reset.data());
                    break;
                case 1:
                    printf("%s %s", lightGrey.data(), reset.data());
                    break;
                case 2:
                    printf("%s %s", darkGrey.data(), reset.data());
                    break;
                case 3:
                    printf("%s %s", black.data(), reset.data());
                    break;
                default:
                    exit(1);
            }
        }
        printf("\n");
    }
    printf("\n\n");
}

void bufferFrame(std::unique_ptr<PPU> & ppu) {
    int cycles = 0;
    while (cycles <= 114 * 144) {
        cycles += 4;
        ppu->update(4);
    }
}

void assertTile00(std::unique_ptr<PPU> & ppu, std::array<char, 64> tile) {
    auto frameBuffer = ppu->getFrameBuffer();
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            uint8_t color = frameBuffer->at(y * 160 + x);
            switch (color) {
                case 0:
                    ASSERT_EQ(tile[y * 8 + x], 'W');
                    break;
                case 1:
                    ASSERT_EQ(tile[y * 8 + x], 'L');
                    break;
                case 2:
                    ASSERT_EQ(tile[y * 8 + x], 'D');
                    break;
                case 3:
                    ASSERT_EQ(tile[y * 8 + x], 'B');
                    break;
                default:
                    exit(1);
            }
        }
        printf("\n");
    }
}

TEST(PPU, Read_single_tile_no_scrolling) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<PPU> ppu = std::make_unique<PPU>(mmu);

    //Set LCDC to use tile map 0 and 8000 addressing mode
    mmu->write(LCDC_ADDRESS, 0xB3);

    //Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    mmu->write(BGP_ADDRESS, 0xE4);

    //Set data of tile 0 in 8000 addressing mode to a test tile:
    std::array<char, 64> startTile = getGTile();
    ppu->loadTileDataTESTING_ONLY(startTile, 0, 1);
    ppu->loadMapTESTING_ONLY(0, 0, 0);

    bufferFrame(ppu);
    printTile00(ppu);
    assertTile00(ppu, startTile);
}

TEST(PPU, Read_single_tile_scrolling_x) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<PPU> ppu = std::make_unique<PPU>(mmu);

    //Set LCDC to use tile map 0 and 8000 addressing mode
    mmu->write(LCDC_ADDRESS, 0xB3);

    //Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    mmu->write(BGP_ADDRESS, 0xE4);

    //Set x-scrolling to 1
    mmu->write(SCX_ADDRESS, 1);

    //Set tile 0 in 8000 addressing mode to the startTile,
    //set tile 1 in 8000 addressing mode to a white tile
    std::array<char, 64> startTile = getGTile();
    std::array<char, 64> whiteTile = getWhiteTile();
    ppu->loadTileDataTESTING_ONLY(startTile, 0, 1);
    ppu->loadTileDataTESTING_ONLY(whiteTile, 1, 1);

    //Set the tile at (0, 0) to the start tile, set tiles at (0, 1) and (1, 0) to white
    fillBackground(ppu, 0, 1);
    ppu->loadMapTESTING_ONLY(0, 0, 0);

    std::array<char, 64> targetTile = getGScrolledXOnceTile();

    bufferFrame(ppu);
    printTile00(ppu);
    //printScreen(ppu);
    assertTile00(ppu, targetTile);
}

TEST(PPU, Many_tiles) {
    std::shared_ptr<MMU> mmu = std::make_shared<MMU>();
    std::unique_ptr<PPU> ppu = std::make_unique<PPU>(mmu);

    //Set LCDC to use tile map 0 and 8000 addressing mode
    mmu->write(LCDC_ADDRESS, 0xB3);

    //Set pallet so that 0=white, 1=light grey, 2=dark grey, 3=black
    mmu->write(BGP_ADDRESS, 0xE4);

    std::array<char, 64> whiteTile = getWhiteTile();
    std::array<char, 64> gTile = getGTile();
    std::array<char, 64> aTile = getATile();
    std::array<char, 64> mTile = getMTile();
    std::array<char, 64> eTile = getETile();
    std::array<char, 64> bTile = getBTile();
    std::array<char, 64> oTile = getOTile();
    std::array<char, 64> yTile = getYTile();

    //Load tile data
    ppu->loadTileDataTESTING_ONLY(whiteTile, 0, 1);
    ppu->loadTileDataTESTING_ONLY(gTile, 1, 1);
    ppu->loadTileDataTESTING_ONLY(aTile, 2, 1);
    ppu->loadTileDataTESTING_ONLY(mTile, 3, 1);
    ppu->loadTileDataTESTING_ONLY(eTile, 4, 1);
    ppu->loadTileDataTESTING_ONLY(bTile, 5, 1);
    ppu->loadTileDataTESTING_ONLY(oTile, 6, 1);
    ppu->loadTileDataTESTING_ONLY(yTile, 7, 1);

    //Load map
    fillBackground(ppu, 0, 0);
    for (int i = 0; i < 7; ++i) {
        ppu->loadMapTESTING_ONLY(0, i, i + 1);
    }

    bufferFrame(ppu);
    printScreen(ppu);
}