/********************************************************************************
 * nrf24l01-ble/at85_i2c.h
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

#ifndef __AT85_I2C_H
#define __AT85_I2C_H

#include <avr/io.h>
#include <util/delay.h>


/**
 * Here SOFT_IIC just for a 6pins I2C/SPI ssd1306 device, It's pinout is "GND,VCC,SCL,SDA,DC,RST" , according to the
 * datasheet said, it's CS pin has already connected to GND. As you can see, they need a short pulse after power-up,
 * but the GPIO for Attiny85 is too expensive, So, here  according to the SSD1306 datasheet, create low level reset circuit
 * to generate a short pulse for reset pin.
 *
 * About the DC Pin. following from SSD1306 datasheet: 8.1.5.2 Write mode for I2C
 *
 *   1) The master device initiates the data communication by a start condition. The definition of the start
 * condition is shown in Figure 8-8. The start condition is established by pulling the SDA from HIGH to
 * LOW while the SCL stays HIGH.
 *
 *   2) The slave address is following the start condition for recognition use. For the SSD1306, the slave
 * address is either “b0111100” or “b0111101” by changing the SA0 to LOW or HIGH (D/C pin acts as SA0).
 *
 *   3) The write mode is established by setting the R/W# bit to logic “0”.
 *
 *   4) An acknowledgement signal will be generated after receiving one byte of data, including the slave
 * address and the R/W# bit. Please refer to the Figure 8-9 for the graphical representation of the
 * acknowledge signal. The acknowledge bit is defined as the SDA line is pulled down during the HIGH
 * period of the acknowledgement related clock pulse.
 *
 *   5) After the transmission of the slave address, either the control byte or the data byte may be sent across
 * the SDA. A control byte mainly consists of Co and D/C# bits following by six “0” ‘s.
 * a. If the Co bit is set as logic “0”, the transmission of the following information will co.......
 *
 * I guess at I2C mode, maybe DC pin can floating.
 */


/**
 * Using the SOFT_IIC means that need a IO for the DC pin.
 * also don't write slave address via the SDA.
 **/
// #define SOFT_IIC 1

#if defined(SOFT_IIC)
    #define DC_PIN           PB4
    #define DC_OUTPUT()      (DDRB  |=  _BV(DC_PIN))
    #define WRITE_CMD()      (PORTB &= ~_BV(DC_PIN))
    #define WRITE_DATA()     (PORTB |=  _BV(DC_PIN))
#endif

#define SCL_PIN  PB2
#define SDA_PIN  PB0

#define SDA_PIN_OUTPUT()   (DDRB  |=  _BV(SDA_PIN))
#define SDA_PIN_INPUT()    (DDRB  &= ~_BV(SDA_PIN))


#define SCL_PIN_OUTPUT()   (DDRB  |= _BV(SCL_PIN))

#define SDA_PIN_LOW()      (PORTB &= ~_BV(SDA_PIN))
#define SDA_PIN_HIGH()     (PORTB |= _BV(SDA_PIN))

#define SCL_PIN_LOW()      (PORTB &= ~_BV(SCL_PIN))
#define SCL_PIN_HIGH()     (PORTB |=  _BV(SCL_PIN))

#define SCL_PIN_READ()     (PINB &  _BV(SCL_PIN))
#define SDA_PIN_READ()     (PINB &  _BV(SDA_PIN))


#define USI_ACK		0
#define USI_DATA	1

#define USI_READ	1
#define USI_WRITE	0
#define NOTLAST		0
#define LAST		1

#define LOW_PERIOD	4
#define HIGH_PERIOD	5

uint8_t i2c_start(uint8_t address,uint8_t mode);
uint8_t i2c_write(uint8_t data);
void i2c_stop(void);
uint8_t i2c_read(uint8_t mode);
uint8_t i2c_transfer (uint8_t data);

#endif /* __AT85_I2C_H */
