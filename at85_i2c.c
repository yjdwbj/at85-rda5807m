/********************************************************************************
 * nrf24l01-ble/at85_i2c.c
 *
 * This file is part of the at85-rda5807m distribution.
 *  (https://github.com/yjdwbj/at85-rda5807m).
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

#include "at85_i2c.h"

void i2c_setup() {

#if defined(SOFT_IIC)

    /* set SDA,SCL,DC to be output */
    DDRB = _BV(SCL_PIN) | _BV(SDA_PIN) | _BV(DC_PIN);

    /* Outputs, clock hold, and start detector disabled. must be quit from SPI mode  */
    USICR = 0;
    USISR = 0;
#else
    DDRB = _BV(SDA_PIN) | _BV(SCL_PIN);
    /* Choosing I2C aka two wire mode */
    USICR |= _BV(USIWM1) | _BV(USIWM0);
    USIDR = 0xff;
    USICR |= _BV(USICLK) | _BV(USIOIE); /* Software stobe as counter clock source */
    USISR = _BV(USISIF) | _BV(USIOIF) | _BV(USIPF) | _BV(USIDC);

#endif
    USICR &= ~_BV(USIWM0);
}

uint8_t i2c_write(uint8_t data) {
#if defined(SOFT_IIC)
    uint8_t i;
    for (i = 0; i < 8; i++) {
        SCL_PIN_LOW();
        if (data & 0x80) {
            SDA_PIN_HIGH();
        } else {
            SDA_PIN_LOW();
        }
        SCL_PIN_HIGH();
        data <<= 1;
    }
#else
    SCL_PIN_LOW();
    USIDR = data; /* set data */

    i2c_transfer(USI_DATA);
    SDA_PIN_INPUT();

    /* read ACK from slave */

    if (i2c_transfer(USI_ACK) & 0x01)
        return 1;
#endif
    return 0;
}

uint8_t i2c_read(uint8_t end) {
    uint8_t data;
    /* set SDA as input */
    SDA_PIN_INPUT();
    // SCL_PIN_LOW();
    data = i2c_transfer(USI_DATA);
    // SDA_PIN_OUTPUT();
    if (end == 0) {
        USIDR = 0xFF;
    } else {
        USIDR = 0x0;
    }
    i2c_transfer(USI_ACK);
    return data;
}

uint16_t i2c_read16()
{
    uint16_t data;
    /* set SDA as input */
    SDA_PIN_INPUT();
    // SCL_PIN_LOW();
    data = i2c_transfer(USI_DATA);
    // SDA_PIN_OUTPUT();
    data <<= 8;
    USIDR = 0x0;
    i2c_transfer(USI_ACK);

    SDA_PIN_INPUT();
    // SCL_PIN_LOW();
    data |= i2c_transfer(USI_DATA);
    // SDA_PIN_OUTPUT();

    USIDR = 0xFF;
    i2c_transfer(USI_ACK);
    return data;
}

uint8_t i2c_master_start() {
    SCL_PIN_HIGH(); // Release SCL
    while (!SCL_PIN_READ())
        ;
    // SDA_PIN_HIGH();
    // while (!SDA_PIN_READ())
    //     ;
    _delay_us(HIGH_PERIOD);
    /* generate start condition */

    SDA_PIN_LOW(); /* Force SDA Low */
    _delay_us(LOW_PERIOD);
    SCL_PIN_LOW(); /* pull SCL Low */
    // _delay_us(HIGH_PERIOD);
    SDA_PIN_HIGH(); /* Release SDA */
}

uint8_t i2c_start(uint8_t address, uint8_t mode) {

    i2c_master_start();
    SCL_PIN_LOW();
    USIDR = (address << 1) | mode;
    i2c_transfer(USI_DATA);

    SDA_PIN_INPUT(); /* set SDA input for read ACK */
    if (i2c_transfer(USI_ACK) & 0x01)
        return 1;
    return 0;
}

void i2c_stop(void) {
    SDA_PIN_LOW();
    SCL_PIN_HIGH(); /** Release SCL */

    while (!SCL_PIN_READ())
        ;

    /** generate STOP */
    _delay_us(LOW_PERIOD);
    SDA_PIN_HIGH(); /** Release SDA */
    _delay_us(HIGH_PERIOD);
}

uint8_t i2c_transfer(uint8_t mode) {
    uint8_t data;
    if (mode == USI_ACK) {
        USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) |
                (1 << USIDC) |    // Prepare register value to: Clear flags, and
                (0xE << USICNT0); // set USI to shift 1 bit i.e. count 2 clock edges.
    } else if (mode == USI_DATA) {
        USISR = (1 << USISIF) | (1 << USIOIF) | (1 << USIPF) |
                (1 << USIDC) |    // Prepare register value to: Clear flags, and
                (0x0 << USICNT0); // set USI to shift 8 bits i.e. count 16 clock edges.
    }

    data = _BV(USIWM1) |               /* Set USI in Two-wire mode */
           _BV(USICS1) | _BV(USICLK) | /* Software clock strobe as source. */
           _BV(USITC);                 /* Toggle Clock Port.  Generate positive SCL edge.*/

    do {
        _delay_us(HIGH_PERIOD);
        USICR = data; /* */
        while (!SCL_PIN_READ())
            ; /* Wait for SCL to go high. */
        _delay_us(LOW_PERIOD);
        USICR = data;
    } while (!(USISR & 1 << USIOIF)); /* Check for transfer complete. */
    _delay_us(HIGH_PERIOD);
    data = USIDR; /* Read out data. */
    USIDR = 0xFF;
    SDA_PIN_OUTPUT(); /* Enable SDA as output. */

    return data;
}