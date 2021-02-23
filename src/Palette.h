//
// Created by Andreas on 2021-02-16.
//

#pragma once

struct rgb {
    float r;
    float g;
    float b;
};

struct Palette {
    rgb c1;
    rgb c2;
    rgb c3;
    rgb c4;
};

const static Palette PALETTE_POCKET = {{0.f, 0.f, 0.f},
                                       {0.33f, 0.33f, 0.33f},
                                       {0.67f, 0.67f, 0.67f},
                                       {1.f, 1.f, 1.f}};

const static Palette PALETTE_DMG = {{0.06f, 0.22f, 0.06f},
                                    {0.19f, 0.38f, 0.19f},
                                    {0.55f, 0.67f, 0.06f},
                                    {0.61f, 0.74f, 0.06f}};

const static Palette PALETTE_DMG_SMOOTH = {{0.03f, 0.09f, 0.13f},
                                           {0.20f, 0.41f, 0.34f},
                                           {0.53f, 0.75f, 0.44f},
                                           {0.88f, 0.97f, 0.82f}};