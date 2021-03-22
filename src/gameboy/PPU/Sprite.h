//
// Created by riddarvid on 3/12/21.
//

#pragma once
#include <cstdint>
#include <memory>
#include <iostream>

class Sprite {
    uint8_t x;
    uint8_t y;
    uint8_t tileIndex;

    union {
        struct {
            unsigned int UNUSED : 4;
            unsigned int paletteNumber : 1;
            unsigned int xFlip : 1;
            unsigned int yFlip : 1;
            unsigned int bgAndWindowOverOBJ : 1;
        };
        uint8_t flags;
    };

public:
    Sprite(uint8_t y, uint8_t x, uint8_t tileIndex, uint8_t flags);

    bool coversLine(uint8_t line, unsigned int objectSize) const;
    bool containsX(uint8_t lcdX) const;
    bool hasHigherPriorityThan(const std::shared_ptr<Sprite>& other) const;

    uint8_t getTileID(uint8_t lcdY) const;
    uint8_t getTileX(uint8_t lcdX) const;
    uint8_t getTileY(uint8_t lcdY) const;
    uint8_t getPaletteNumber() const;
    bool backgroundOverSprite() const;

    void print() const;
};
