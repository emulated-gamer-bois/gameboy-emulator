//
// Created by Andreas on 2021-02-28.
//

#pragma once
#define LCD_WIDTH 160
#define LCD_HEIGHT 144
#define LCD_REFRESH_RATE 60

#define V_BLANK_IF_BIT (1 << 0)
#define STAT_IF_BIT (1 << 1)
#define TIMER_IF_BIT (1 << 2)
#define SERIAL_IF_BIT (1 << 3)
#define CONTROLLER_IF_BIT (1 << 4)
