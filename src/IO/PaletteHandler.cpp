#include "PaletteHandler.h"

PaletteHandler::PaletteHandler() {
    palettes[0].paletteName = "GB DMG-01";
    palettes[0].palette = {{0.06f, 0.22f, 0.06f},
                           {0.19f, 0.38f, 0.19f},
                           {0.55f, 0.67f, 0.06f},
                           {0.61f, 0.74f, 0.06f}};
    palettes[1].paletteName = "GB Pocket";
    palettes[1].palette = {{0.12f, 0.12f, 0.12f},
                           {0.30f, 0.33f, 0.23f},
                           {0.54f, 0.58f, 0.47f},
                           {0.75f, 0.81f, 0.63f}};
    palettes[2].paletteName = "Special Smooth DMG-01";
    palettes[2].palette = {{0.03f, 0.09f, 0.13f},
                           {0.20f, 0.41f, 0.34f},
                           {0.53f, 0.75f, 0.44f},
                           {0.88f, 0.97f, 0.82f}};
}

const std::string PaletteHandler::getPaletteName(int index) const {
    return palettes[index].paletteName;
}

const Palette PaletteHandler::getPalette(int index) const {
    return palettes[index].palette;
}

const int PaletteHandler::getPaletteAmount() const {
    return paletteAmount;
}