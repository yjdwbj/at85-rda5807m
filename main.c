/********************************************************************************
 * nrf24l01-ble/main.c
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

#include "at85_ir.h"
#include "oled.h"
#include "rda5807m.h"
#include <avr/io.h>
#define LCD_BUFFER_SIZE 16
extern uint8_t lcd_buffer[LCD_BUFFER_SIZE];

int main(void) {

    oled_init();
    oled_clear();

    _delay_ms(1000);
    init_fm();
    set_frequency(8750);
    show_radio_info();
    sei(); //  AVR Status Register, Bit 7 – I: Global Interrupt Enable
    ir_bus_init();
    while (1) {
        ir_data_ready();
    }

    return 1;
}
