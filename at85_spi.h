/********************************************************************************
 * nrf24l01-ble/at85_spi.h
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

#ifndef __AT85_SPI_H
#define __AT85_SPI_H

#include <avr/io.h>
#include <util/delay.h>

#define SPI_CSN  PB3
#define SPI_SCK  PB2
#define SPI_MISO PB1
#define SPI_MOSI PB0

/***
 *
 * PORTB: Depends on the configuration of the DDRB. If DDRB is an input,
 *        PORTB sets the internal Pull-up resistors (0=off; 1=on). If DDRB is an output,
 *        PORTB sets high/low outputs (0=low; 1=high). Single ports are addressed by PORTB0-PORTB5.
 * DDRB: Selects the direction of a pin (0=Input; 1=Output). Single pins are addressed by DDB0-DDB5.
 * PINB: A logical 1 toggles the accroding bit in PORTB. Single port input pins are addressed by PIN0-PIN5.
 */

#define CSN_LOW()      (PORTB &= ~_BV(SPI_CSN))
#define CSN_HIGH()     (PORTB |=  _BV(SPI_CSN))

void spi_setup();
void spi_write(uint8_t byte);
void spi_sendcmd(uint8_t cmd, uint8_t data);
void spi_sendblock(uint8_t* buf,uint8_t length);
void spi_dosend(uint8_t cmd,uint8_t *buf, uint8_t len);

#endif /** __AT85_SPI_H */