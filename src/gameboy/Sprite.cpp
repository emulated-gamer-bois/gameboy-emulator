//
// Created by riddarvid on 3/12/21.
//

#include "Sprite.h"

Sprite::Sprite(uint8_t y, uint8_t x, uint8_t tileIndex, uint8_t flags) {
    this->x = x;
    this->y = y;
    this->tileIndex = tileIndex;
    this->flags = flags;
}

bool Sprite::coversLine(uint8_t line, unsigned int objectSize) const {
    uint8_t min = this->y - 16;
    uint8_t max;
    if (objectSize) {
        max = min + 16;
    } else {
        max = min + 8;
    }
    return min <= line && line < max;
}

bool Sprite::containsX(uint8_t lcdX) const {
    return lcdX > (this->x - 8) && lcdX < this->x;
}

uint8_t Sprite::getTileID(uint8_t lcdY) const {
    if (lcdY - (this->y - 16) < 8) {
        return tileIndex;
    }
    return tileIndex + 1;
}

uint8_t Sprite::getTileX(uint8_t lcdX) const {
    return lcdX - (this->x - 8);
}

uint8_t Sprite::getTileY(uint8_t lcdY) const {
    uint8_t spriteY = lcdY - (this->y - 16);
    if (spriteY < 8) {
        return spriteY;
    }
    return spriteY - 8;
}

bool Sprite::hasHigherPriorityThan(const std::shared_ptr<Sprite>& other) const {
    if (other == nullptr) {
        return true;
    }
    return this->x < other->x;
}

uint8_t Sprite::getPaletteNumber() const {
    return paletteNumber;
}

void Sprite::print() {
    std::cout << "X: " << (int)this->x << " Y: " << (int)this->y << " Tile: " << (int)tileIndex << " Flags: " << (int)this->flags << std::endl;
}
