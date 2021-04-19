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
    palettes[2].paletteName = "GB Light";
    palettes[2].palette = {{0.f, 0.31f, 0.23f},
                           {0.f, 0.41f, 0.29f},
                           {0.f, 0.6f, 0.44f},
                           {0.f, 0.71f, 0.51f}};
    palettes[3].paletteName = "GBC Brown";
    palettes[3].palette = {{0.f, 0.f, 0.f},
                           {0.52f, 0.26f, 0.02f},
                           {0.93f, 0.60f, 0.33f},
                           {1.f, 1.f, 1.f}};
    palettes[4].paletteName = "GBC Pastel Mix";
    palettes[4].palette = {{0.f, 0.f, 0.f},
                           {0.61f, 0.57f, 0.96f},
                           {0.93f, 0.54f, 0.55f},
                           {0.99f, 0.98f, 0.67f}};
    palettes[5].paletteName = "GBC Blue";
    palettes[5].palette = {{0.f, 0.f, 0.f},
                           {0.02f, 0.2f, 0.99f},
                           {0.45f, 0.67f, 0.99f},
                           {1.f, 1.f, 1.f}};
    palettes[6].paletteName = "GBC Green";
    palettes[6].palette = {{0.f, 0.f, 0.f},
                           {0.99f, 0.2f, 0.02f},
                           {0.02f, 0.98f, 0.02f},
                           {1.f, 1.f, 1.f}};
    palettes[7].paletteName = "GBC Red";
    palettes[7].palette = {{0.f, 0.f, 0.f},
                           {0.67f, 0.15f, 0.14f},
                           {0.93f, 0.54f, 0.55f},
                           {1.f, 1.f, 1.f}};
    palettes[8].paletteName = "GBC Orange";
    palettes[8].palette = {{0.f, 0.f, 0.f},
                           {0.99f, 0.2f, 0.02f},
                           {0.96f, 0.99f, 0.02f},
                           {1.f, 1.f, 1.f}};
    palettes[9].paletteName = "GBC Dark Blue";
    palettes[9].palette = {{0.f, 0.f, 0.f},
                           {0.27f, 0.2f, 0.64f},
                           {0.61f, 0.57f, 0.96f},
                           {1.f, 1.f, 1.f}};
    palettes[10].paletteName = "GBC Dark Green";
    palettes[10].palette = {{0.f, 0.f, 0.f},
                            {0.02f, 0.2f, 0.99f},
                            {0.02f, 0.98f, 0.02f},
                            {1.f, 1.f, 1.f}};
    palettes[11].paletteName = "GBC Dark Brown";
    palettes[11].palette = {{0.32f, 0.16f, 0.13f},
                           {0.48f, 0.35f, 0.26f},
                           {0.78f, 0.59f, 0.58f},
                           {0.96f, 0.86f, 0.83f}};
    palettes[12].paletteName = "GBC Yellow";
    palettes[12].palette = {{0.f, 0.f, 0.f},
                            {0.52f, 0.26f, 0.02f},
                            {0.96f, 0.99f, 0.02f},
                            {1.f, 1.f, 1.f}};
    palettes[13].paletteName = "GBC Greyscale";
    palettes[13].palette = {{0.f, 0.f, 0.f},
                           {0.32f, 0.32f, 0.32f},
                           {0.64f, 0.64f, 0.64f},
                           {1.f, 1.f, 1.f}};
    palettes[14].paletteName = "GBC Inverted";
    palettes[14].palette = {{1.f, 1.f, 1.f},
                           {0.96f, 0.99f, 0.02f},
                           {0.02f, 0.64f, 0.64f},
                           {0.f, 0.f, 0.f}};
    palettes[15].paletteName = "SGB 1A";
    palettes[15].palette = {{0.19f, 0.09f, 0.31f},
                            {0.66f, 0.16f, 0.13f},
                            {0.85f, 0.56f, 0.28f},
                            {0.97f, 0.91f, 0.78f}};
    palettes[16].paletteName = "SGB 1B";
    palettes[16].palette = {{0.f, 0.f, 0.f},
                            {0.69f, 0.31f, 0.06f},
                            {0.78f, 0.69f, 0.44f},
                            {0.85f, 0.85f, 0.75f}};
    palettes[17].paletteName = "SGB 1C";
    palettes[17].palette = {{0.22f, 0.22f, 0.6f},
                            {0.6f, 0.22f, 0.38f},
                            {0.91f, 0.6f, 0.31f},
                            {0.97f, 0.75f, 0.97f}};
    palettes[18].paletteName = "SGB 1D";
    palettes[18].palette = {{0.31f, 0.09f, 0.f},
                            {0.97f, 0.f, 0.f},
                            {0.75f, 0.5f, 0.28f},
                            {0.97f, 0.97f, 0.66f}};
    palettes[19].paletteName = "SGB 1E";
    palettes[19].palette = {{0.35f, 0.22f, 0.13f},
                            {0.41f, 0.53f, 0.25f},
                            {0.47f, 0.75f, 0.47f},
                            {0.97f, 0.85f, 0.69f}};
    palettes[20].paletteName = "SGB 1F";
    palettes[20].palette = {{0.f, 0.25f, 0.06f},
                            {0.66f, 0.f, 0.f},
                            {0.88f, 0.53f, 0.31f},
                            {0.85f, 0.91f, 0.97f}};
    palettes[21].paletteName = "SGB 1G";
    palettes[21].palette = {{0.97f, 0.97f, 0.35},
                            {0.47f, 0.47f, 0.f},
                            {0.f, 0.63f, 0.91f},
                            {0.f, 0.f, 0.31f}};
    palettes[22].paletteName = "SGB 1H";
    palettes[22].palette = {{0.19f, 0.09f, 0.f},
                            {0.5f, 0.25f, 0.f},
                            {0.97f, 0.72f, 0.53f},
                            {0.97f, 0.91f, 0.88f}};

}

const std::string PaletteHandler::getPaletteName(int index) const {
    return palettes[index].paletteName;
}

const Palette PaletteHandler::getPalette(int index) const {
    return palettes[index].palette;
}

const int PaletteHandler::getPaletteAmount() const {
    return PALETTE_AMOUNT;
}