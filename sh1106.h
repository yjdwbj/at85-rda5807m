/********************************************************************************
 * nrf24l01-ble/sh1106.h
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

#ifndef __SH1106_H
#define __SH1106_H

#include "at85_i2c.h"
#include <avr/pgmspace.h>

/** defines according to datasheet
 * "132 X 64 Dot Matrix OLED/PLED Segment/Common Driver with Controller".pdf
 **/

/* Set Lower Column Address: (00H - 0FH) */
#define SH1106_SETLOWCOLUMN        0x00

/* Set Higher Column Address: (10H - 1FH) */
#define SH1106_SETHIGHCOLUMN       0x10

/* 30H - 33H, A1A0: 00 = 7.4; 01 = 8.0; 10 = 8.4; 11 = 9.0 */
#define SH1106_SETPUMPVOLTAGEVALE  0x30

/* Set Display Start Line: (40H - 7FH) */
#define SH1106_SETSTARTLINE        0x40

/* Contrast Data Register Set: (00H - FFH) */
#define SH1106_SETCONTRAST         0x81

/* Set Segment Re-map: (A0H - A1H) */
#define SH1106_SEGREMAP            0xA0

/* Set Entire Display OFF */
#define SH1106_DISPLAYALLON_RESUME 0xA4

/* Set Entire Display ON */
#define SH1106_DISPLAYALLON        0xA5

/* Set Normal */
#define SH1106_NORMALDISPLAY       0xA6

/* Set Reverse Display */
#define SH1106_INVERTDISPLAY       0xA7

/* Multiplex Ration Data Set: (00H - 3FH) */
#define SH1106_SETMULTIPLEX        0xA8

/* DC-DC Control Mode Set: (ADH), */
#define SH1106_SETDCDCCTLMODE      0xAD

/* DC-DC ON/OFF Mode Set: (8AH - 8BH) */
#define SH1106_SETDCDCMODEOFF      0x8A
#define SH1106_SETDCDCMODEON       0x8B


/* Display OFF/ON: (AEH - AFH) */
#define SH1106_DISPLAYOFF          0xAE
#define SH1106_DISPLAYON           0xAF

/* Set Page Address: (B0H - B7H) */
#define SH1106_SETPAGEADDRESS      0xB0

/* Scan from COM0 to COM [N -1] */
#define SH1106_COMSCANINC          0xC0

/* When D = “H”, Scan from COM [N -1] to COM0. */
#define SH1106_COMSCANDEC          0xC8

/* Display Offset Mode Set: (D3H),range is (00H~3FH) */
#define SH1106_DISPLAYOFFSETMODE   0xD3

/*
 * Set Display Clock Divide Ratio/Oscillator Frequency: (Double Bytes Command)
 * Divide Ratio/Oscillator Frequency Mode Set: (D5H)
 * Divide Ratio/Oscillator Frequency Data Set: (00H - 3FH)
 *     A3 - A0 defines the divide ration of the display clocks (DCLK).
 *             Divide Ration = A[3:0]+1.
 *     A7 - A4 sets the oscillator frequency. Oscillator frequency increase
 *             with the value of A[7:4] and vice versa.
 * */
#define SH1106_SETDISPLAYCLOCKDIV  0xD5


/**
 * Set Dis-charge/Pre-charge Period: (Double Bytes Command)
 * Pre-charge Period Mode Set: (D9H)
 * Dis-charge/Pre-charge Period Data Set: (00H - FFH)
 * Pre-charge Period Adjust: (A3 - A0)
 * Dis-charge Period Adjust: (A7 - A4)
 */
#define SH1106_SETCHARGEPMS        0xD9


/**
 * Set Common pads hardware configuration: (Double Bytes Command)
 * Common Pads Hardware Configuration Mode Set: (DAH)
 * Sequential/Alternative Mode Set: (02H - 12H)
 */
#define SH1106_SETCOMPINS          0xDA

/**
 * Set VCOM Deselect Level: (Double Bytes Command)
 * VCOM Deselect Level Mode Set: (DBH)
 * VCOM Deselect Level Data Set: (00H - FFH)
 */
#define SH1106_SETVCOMDETECT       0xDB

/* Read-Modify-Write: (E0H) */
#define SH1106_READMODWRITE        0xE0

/* End: (EEH) */
#define SH1106_READMODEEND         0xEE

/* Non-Operation Command. */
#define SH1106_NOP                 0xE3

/**
 *  A0,RD,WR
 *  001 = read status
 *  010 = read modify write
 *  110 = Write ram data
 *  101 = read display data
 */


#define SH1106_ADDRESS            0x3C

#define SH1106_WIDTH               132
#define SH1106_HEIGHT               64


#define USI_READ	1
#define USI_WRITE	0

#define sh1106_senddata(data)  (i2c_write(data))

void sh1106_init(void);
static void sh1106_sendcmd(uint8_t cmd);
void sh1106_setpos(uint8_t x,uint8_t y);
void sh1106_clear(void);
void sh1106_fillscreen(uint8_t);

#endif /* __SH1106_H */
