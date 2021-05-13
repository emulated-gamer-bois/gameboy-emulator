#include "Sprite.h"
#include <iostream>

Sprite::Sprite(uint8_t y, uint8_t x, uint8_t tileIndex, uint8_t flags, uint8_t positionInOAM) {
    this->x = x;
    this->y = y;
    this->tileIndex = tileIndex;
    this->flags = flags;
    this->positionInOAM = positionInOAM;
}

int Sprite::getX() const {
    //The x variable refers to the x-coordinate of the sprite's top left corner + 8, therefore we subtract 8
    return (int)this->x - 8;
}

int Sprite::getY() const {
    //The x variable refers to the y-coordinate of the sprite's top left corner + 16, therefore we subtract 16
    return (int)(this->y) - 16;
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

uint8_t Sprite::getTileID(uint8_t lcdY, uint8_t objectSize) const {
    if (objectSize == 0) { //If the object size is 0, then the tile ID is simply the tile index
        return tileIndex;
    }
    //If the object size is 1, then the tile index refers to a pair of tiles.
    //Which tile is used depends on the sprites position relative to the scanline.
    uint8_t tileY = lcdY - getY();
    if ((tileY < 8 && !yFlip) || (tileY >= 8 && yFlip)) {
        return tileIndex & 0xFE;
    }
    return tileIndex | 0x01;
}

uint8_t Sprite::getPaletteNumber() const {
    return paletteNumber;
}

bool Sprite::isBackgroundOverSprite() const {
    return bgAndWindowOverOBJ;
}

bool Sprite::coversLine(uint8_t lineY, unsigned int objectSize) const {
    uint8_t min = getY();
    uint8_t distance = lineY - min;
    if (objectSize) {
        return distance < 16;
    } else {
        return distance < 8;
    }
}

bool Sprite::hasHigherPriorityThan(const Sprite &other) const {
    if (this->x != other.x) { //Sprite with lower x-position has priority
        return this->x < other.x;
    }
    return this->positionInOAM < other.positionInOAM; //If same x-position first sprite in OAM has priority
}

bool operator<(const Sprite &lhs, const Sprite &rhs) {
    return lhs.hasHigherPriorityThan(rhs);
}

void Sprite::print() const {
    std::cout << "X: " << (int) this->x << " Y: " << (int) this->y << " Tile: " << (int) tileIndex << " Flags: "
              << (int) this->flags << std::endl;
}
