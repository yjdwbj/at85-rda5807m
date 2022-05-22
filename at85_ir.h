/********************************************************************************
 * at85-rda5807m/at85_i2c.h
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

#ifndef __AT85_IR_H
#define __AT85_IR_H

#include <avr/interrupt.h>
#include <stdbool.h>

typedef enum {
    WAIT_STATE,
    LEADING_PULSE_STATE,
    LEADING_SPACE_STATE,
    DATA_STATE,
    FINAL_PULSE_STATE
} IRCaptureState;

typedef enum {
    // Newly captured IR data in pulse buffer
    BUF_READY,
    // Capturing IR repeat code (IR data still captured in pulse buffer)
    BUF_REPEAT,
    /*
        Nothing is happening (timeout) /
        receiving error /
        only if disabled repetition: pulse buffer already processed in IR_data_ready() function
    */
    BUF_NOT_READY
} PulseBufferState;


static volatile PulseBufferState bufferState = BUF_NOT_READY;

void tc0_init(void);
void tc1_init(void);
void ir_bus_init(void);
bool ir_data_ready(void);
void ir_data_clean(void);
static uint8_t check_low_time(bool isLeading);
static uint8_t check_high_time(bool isLeading);

#endif /** __AT85_IR_H */