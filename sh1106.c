/********************************************************************************
 * nrf24l01-ble/sh1106.c
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

#include "sh1106.h"

const uint8_t sh1106_init_sequence[] PROGMEM = {
    SH1106_DISPLAYOFF,
    SH1106_DISPLAYALLON_RESUME,

    /* Multiplex Ratio Data Set: 64 (POR = 0x3f, 64) */
    SH1106_SETMULTIPLEX,0x3f,

    /* Display OffsetData Set: 0 (POR = 0x00) */
    SH1106_DISPLAYOFFSETMODE,0x00,

    SH1106_SETSTARTLINE,

    SH1106_SEGREMAP + 1,

    SH1106_COMSCANDEC,

    /* DC-DC ON/OFF Mode Set: Built-in DC-DC is used, Normal Display (POR = 0x8b) */
    SH1106_SETDCDCCTLMODE,SH1106_SETDCDCMODEON,

    /* Dis-charge/Pre-charge PeriodData Set: pre-charge 2 DCLKs,
     * dis-charge 2 DCLKs (POR = 0x22, pre-charge 2 DCLKs, dis-charge 2 DCLKs)
     * */
    SH1106_SETDISPLAYCLOCKDIV,0xf0,

    /* VCOM Deselect LevelData Set: 0,770V (POR = 0x35, 0,770 V) */
    SH1106_SETVCOMDETECT,0x20,

    /* Set Pump voltage value: 8,0 V (POR = 0x32, 8,0 V) */
    SH1106_SETPUMPVOLTAGEVALE | 0x0,

    /* Contrast Data Register Set: 255 (large) (POR = 0x80) */
    SH1106_SETCONTRAST,0x7f,

    SH1106_NORMALDISPLAY,

    /* com pin HW config, sequential com pin config (bit 4), disable left/right remap (bit 5) */
    SH1106_SETCOMPINS, 0x12,
    SH1106_DISPLAYON
};

void sh1106_init(void)
{
    uint8_t i;
    i2c_setup();
    for(i=0; i < sizeof(sh1106_init_sequence);i++)
    {
      sh1106_sendcmd(pgm_read_byte(&sh1106_init_sequence[i]));
    }
}

static void sh1106_sendcmd(uint8_t cmd)
{
    i2c_start(SH1106_ADDRESS,USI_WRITE);
    i2c_write(0);
    i2c_write(cmd);
    i2c_stop();
}

void sh1106_fillscreen(uint8_t data)
{
  uint8_t y,x;

	for(y = 0; y < 8; y++)
	{
		sh1106_sendcmd(SH1106_SETPAGEADDRESS+y);
		sh1106_sendcmd(0x1);
		sh1106_sendcmd(SH1106_SETHIGHCOLUMN);
    i2c_start(SH1106_ADDRESS,USI_WRITE);
    i2c_write(0x40);
    for(x = 0;x < SH1106_WIDTH; x++)
    {
      sh1106_senddata(data);
    }
    i2c_stop();
	}
}

void sh1106_setpos(uint8_t x,uint8_t y)
{
    sh1106_sendcmd(SH1106_SETPAGEADDRESS + y);
    sh1106_sendcmd(SH1106_SETHIGHCOLUMN |(( x & 0xf0) >> 4));
    sh1106_sendcmd(0x1 | (x & 0x0f));
}

void sh1106_clear(void)
{
  sh1106_fillscreen(0);
}
