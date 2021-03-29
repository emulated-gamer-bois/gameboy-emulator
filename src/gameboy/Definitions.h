//
// Created by Andreas on 2021-02-28.
//

#pragma once
#define EMULATOR_NAME_STRING "Lame Boy"
#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define BACKGROUND_WIDTH 256
#define BACKGROUND_HEIGHT 256
#define LCD_REFRESH_RATE 60
#define MIN_SCREEN_MULTIPLIER 4
#define MIN_EMULATION_SPEED_FLOAT 0.25f
#define MAX_EMULATION_SPEED_FLOAT 8.f
#define KEY_INDEX_JOYPAD_START 0
#define KEY_INDEX_SPECIAL_START 8

#define V_BLANK_IF_BIT      (1 << 0)
#define STAT_IF_BIT         (1 << 1)
#define TIMER_IF_BIT        (1 << 2)
#define SERIAL_IF_BIT       (1 << 3)
#define CONTROLLER_IF_BIT   (1 << 4)
