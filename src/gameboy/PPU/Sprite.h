#pragma once
#include <cstdint>

class Sprite {
    uint8_t x;
    uint8_t y;
    uint8_t tileIndex;
    uint8_t positionInOAM;

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
    Sprite(uint8_t y, uint8_t x, uint8_t tileIndex, uint8_t flags, uint8_t positionInOAM);

    /**
     * @return the x-coordinate of the sprite's top left corner.
     */
    int getX() const;
    /**
     * @return the y-coordinate of the sprite's top left corner.
     */
    int getY() const;
    /**
     * Given a x-coordinate, determines which x-coordinate of the sprite it intersects.
     * @param lcdX a given x-coordinate.
     * @return the x-coordinate of the sprite that intersects lcdX.
     */
    uint8_t getTileX(uint8_t lcdX) const;
    /**
     * Given a y-coordinate, determines which y-coordinate of the sprite it intersects.
     * @param lcdY a given y-coordinate.
     * @return the y-coordinate of the sprite that intersects lcdY.
     */
    uint8_t getTileY(uint8_t lcdY) const;
    /**
     * Given a y-coordinate and the object size, determines the tile ID of the tile that intersects that y-coordinate.
     * @param lcdY a given y-coordinate.
     * @param objectSize a value that is 0 if sprites are composed of one tile and 1 if it is composed of two tiles.
     * @return the tile ID of the tile that intersects the y-coordinate.
     */
    uint8_t getTileID(uint8_t lcdY, uint8_t objectSize) const;
    /**
     * @return the palette number associated with the sprite.
     */
    uint8_t getPaletteNumber() const;
    /**
     * If the value is 0, the sprite has priority. Otherwise, the background has priority.
     * @return the sprite's drawing priority over the background.
     */
    bool isBackgroundOverSprite() const;

    /**
     * Determines whether the sprite intersects a given horizontal line.
     * @param lineY the y-coordinate of the line.
     * @param objectSize a value that is 0 if sprites are composed of one tile and 1 if it is composed of two tiles.
     * @return true if the sprite intersects the line, false otherwise.
     */
    bool coversLine(uint8_t lineY, unsigned int objectSize) const;
    /**
     * Determines whether the sprite has higher drawing priority than another sprite.
     * @param other the sprite we're comparing this to.
     * @return true if this sprite has higher priority than the other sprite.
     */
    bool hasHigherPriorityThan(const Sprite &other) const;
    /**
     * The less than operator, defined using drawing priority.
     * @param lhs left hand side operand.
     * @param rhs right hand side operand.
     * @return true if lhs has higher priority than rhs.
     */
    friend bool operator <(const Sprite & lhs, const Sprite & rhs);

    /**
     * Utility method. Prints coordinates, tileID and flags.
     */
    void print() const;
};