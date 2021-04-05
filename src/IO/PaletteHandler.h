#pragma once

#include <string>
#include <array>

#include "Palette.h"

class PaletteHandler {
public:
    PaletteHandler();
    [[nodiscard]] const std::string getPaletteName(int index) const;
    [[nodiscard]] const Palette getPalette(int index) const;
    [[nodiscard]] const int getPaletteAmount() const;

private:
    struct PaletteEntry {
        std::string paletteName;
        Palette palette;
    };

    const static int paletteAmount = 15;

    std::array<PaletteEntry, paletteAmount> palettes;
};
