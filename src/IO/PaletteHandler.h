#pragma once

#include <string>
#include <array>

#include "Palette.h"

class PaletteHandler {
public:
    static const int paletteAmount = 23;

    PaletteHandler();
    [[nodiscard]] const std::string getPaletteName(int index) const;
    [[nodiscard]] const Palette getPalette(int index) const;

private:
    struct PaletteEntry {
        std::string paletteName;
        Palette palette;
    };

    std::array<PaletteEntry, paletteAmount> palettes;
};
