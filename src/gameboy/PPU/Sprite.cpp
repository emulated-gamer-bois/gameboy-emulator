#include "Sprite.h"
#include <iostream>

Sprite::Sprite(uint8_t y, uint8_t x, uint8_t tileIndex, uint8_t flags, uint8_t positionInOAM) {
    this->x = x;
    this->y = y;
    this->tileIndex = tileIndex;
    this->flags = flags;
    this->positionInOAM = positionInOAM;
}

bool Sprite::coversLine(uint8_t line, unsigned int objectSize) const {
    uint8_t min = getY();
    uint8_t distance = line - min;
    if (objectSize) {
        return distance < 16;
    } else {
        return distance < 8;
    }
}


bool Sprite::hasHigherPriorityThan(const Sprite &other) const {
    if (this->x != other.x) {
        return this->x < other.x;
    }
    return this->positionInOAM < other.positionInOAM;
}

uint8_t Sprite::getTileID(uint8_t lcdY, uint8_t objectSize) const {
    if (objectSize == 0) {
        return tileIndex;
    }
    uint8_t tileY = lcdY - getY();
    if ((tileY < 8 && !yFlip) || (tileY >= 8 && yFlip)) {
        return tileIndex & 0xFE;
    }
    return tileIndex | 0x01;
}

uint8_t Sprite::getTileX(uint8_t lcdX) const {
    uint8_t tileX = lcdX - getX();
    if (xFlip) {
        return 7 - tileX;
    }
    return tileX;
}

uint8_t Sprite::getTileY(uint8_t lcdY) const {
    uint8_t tileY = lcdY - getY(); //Between 0-15
    if (tileY >= 8) {
        tileY = tileY - 8;
    }
    if (yFlip) {
        tileY = 7 - tileY;
    }
    return tileY;
}

uint8_t Sprite::getPaletteNumber() const {
    return paletteNumber;
}

bool Sprite::backgroundOverSprite() const {
    return bgAndWindowOverOBJ;
}

void Sprite::print() const {
    std::cout << "X: " << (int) this->x << " Y: " << (int) this->y << " Tile: " << (int) tileIndex << " Flags: "
              << (int) this->flags << std::endl;
}

int Sprite::getX() const {
    return (int)this->x - 8;
}

int Sprite::getY() const {
    return (int)(this->y) - 16;
}

bool operator<(const Sprite &lhs, const Sprite &rhs) {
    return lhs.hasHigherPriorityThan(rhs);
}


