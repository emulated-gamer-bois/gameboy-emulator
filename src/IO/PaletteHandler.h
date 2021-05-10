/**
 * This class contains all palettes that can be used when rendering the screen. The palettes is based on hardware
 * palettes and on the palettes of systems like the Game Boy Color and the Super Game Boy. A palette is described
 * by a name and four colors and is stored sequentially.
 */

#pragma once

#include <string>
#include <array>

#include "Palette.h"

class PaletteHandler {
public:
    static const int paletteAmount = 23;

    PaletteHandler();

    /**
     * Getter for the name of the palette at index.
     *
     * @param index
     * @return the name of the palette at index
     */
    [[nodiscard]] std::string getPaletteName(int index) const;

    /**
     * Getter for the palette at index.
     *
     * @param index
     * @return palette at index
     */
    [[nodiscard]] Palette getPalette(int index) const;

private:
    struct PaletteEntry {
        std::string paletteName;
        Palette palette;
    };

    std::array<PaletteEntry, paletteAmount> palettes;
};
