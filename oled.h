/********************************************************************************
 * nrf24l01-ble/oled.h
 *
 * This file is part of the at85_btle distribution.
 *  (https://github.com/yjdwbj/at85_btle).
 * Copyright (c) 2021 Liu Chun Yang
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 ********************************************************************************/

#ifndef __OLED_H
#define __OLED_H

#include "at85_i2c.h"
#define USE_SH1106 1
// #define USE_SSD1306 1


#define oled_datastop()  i2c_stop()
#define oled_stop()      i2c_stop()
#if defined(USE_SSD1306) && !defined(USE_SH1106)
    #include "ssd1306.h"
    #define OLED_ADDRESS            SSD1306_ADDRESS
    #define OLED_WIDTH              SSD1306_WIDTH
    #define oled_init()             ssd1306_init()
    #define oled_senddata(data)     ssd1306_senddata(data)
    #define oled_sendcmd(cmd)       ssd1306_sendcmd(cmd)
    #define oled_setpos(x,y)        ssd1306_setpos(x,y)
    #define oled_fillscreen(data)   ssd1306_fillscreen(data)
    #define oled_clear()            ssd1306_clear()
#elif defined(USE_SH1106) && !defined(USE_SSD1306)
    #include "sh1106.h"
    #define OLED_ADDRESS            SH1106_ADDRESS
    #define OLED_WIDTH              SH1106_WIDTH
    #define oled_init()             (sh1106_init())
    #define oled_senddata(data)     (sh1106_senddata(data))
    #define oled_sendcmd(cmd)       (sh1106_sendcmd(cmd))
    #define oled_setpos(x,y)        (sh1106_setpos(x,y))
    #define oled_fillscreen(data)   (sh1106_fillscreen(data))
    #define oled_clear()            (sh1106_clear())
#else
    #error both can't be defined or undefined same time
#endif


#define USI_READ	1
#define USI_WRITE	0

#include <avr/io.h>
#include <avr/pgmspace.h>

void oled_p6x8str(uint8_t x, uint8_t y, uint8_t ch[]);
void oled_p8x16str(uint8_t x,uint8_t y,uint8_t ch[]);

#endif