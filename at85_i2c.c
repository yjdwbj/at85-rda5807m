/********************************************************************************
 * nrf24l01-ble/at85_i2c.c
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

#include "at85_i2c.h"

void i2c_setup()
{

#if defined(SOFT_IIC)

  /* set SDA,SCL,DC to be output */
  DDRB = _BV(SCL_PIN)| _BV(SDA_PIN) | _BV(DC_PIN);

  /* Outputs, clock hold, and start detector disabled. must be quit from SPI mode  */
  USICR = 0;
  USISR = 0;
#else
  DDRB  = _BV(SDA_PIN) | _BV(SCL_PIN);
  SDA_PIN_HIGH();
  SCL_PIN_HIGH();
  USICR = _BV(USIWM1) |  _BV(USIWM0)|  /* Choosing I2C aka two wire mode */
          _BV(USICLK);   /* Software stobe as counter clock source */
  USISR = _BV(USISIF) | _BV(USIOIF) | _BV(USIPF) | _BV(USIDC);
  USIDR = 0xff;
#endif
  USICR &= ~_BV(USIWM0);
}


uint8_t i2c_write( uint8_t data)
{
#if defined(SOFT_IIC)
    uint8_t i;
    for(i = 0; i < 8; i++)
    {
        SCL_PIN_LOW();
        if (data & 0x80)
        {
            SDA_PIN_HIGH();
        }
        else
        {
            SDA_PIN_LOW();
        }
        SCL_PIN_HIGH();
        data <<= 1;
    }
#else
    SCL_PIN_LOW();
    USIDR = data;   /* set data */

    i2c_transfer(USI_DATA);
    SDA_PIN_INPUT();

    /* read ACK from slave */

    if (i2c_transfer(USI_ACK) & 0x01) return 1;
#endif
    return 0;
}

uint8_t i2c_read(uint8_t mode)
{
    /* read a byte */
    SDA_PIN_INPUT();
    uint8_t data = i2c_transfer(USI_DATA);
    if(mode == NOTLAST)
    {
        USIDR = 0x7f;
    } else {
        USIDR = 0xff;
    }

    i2c_transfer(USI_ACK);
    return data;
}


uint8_t i2c_start(uint8_t address,uint8_t mode)
{
    SCL_PIN_HIGH(); // Release SCL
    while(!SCL_PIN_READ());
    SDA_PIN_HIGH();
    while (!SDA_PIN_READ());


    /* generate start condition */

    SDA_PIN_LOW(); /* Force SDA Low */
    _delay_us(LOW_PERIOD);
    SCL_PIN_LOW(); /* pull SCL Low */
    _delay_us(HIGH_PERIOD);
    SDA_PIN_HIGH(); /* Release SDA */

    SCL_PIN_LOW();
    USIDR = (address << 1) + mode;
    i2c_transfer(USI_DATA);

    SDA_PIN_INPUT(); /* set SDA input for read ACK */

    if(i2c_transfer(USI_ACK) & 0x01) return 1;
    return 0;
}

void i2c_stop(void)
{
    SDA_PIN_LOW();
    SCL_PIN_HIGH();  /** Release SCL */

    while(!SCL_PIN_READ());

    /** generate STOP */
    _delay_us(LOW_PERIOD);
    SDA_PIN_HIGH(); /** Release SDA */
    _delay_us(HIGH_PERIOD);
}

uint8_t i2c_transfer(uint8_t mode)
{
    uint8_t data;
    if(mode == USI_ACK)
    {
        USISR = 0xfe;
    }else if(mode == USI_DATA)
    {
        USISR = 0xf0;
    }

    data = _BV(USIWM1) |  /* Set USI in Two-wire mode */
           _BV(USICS1) |_BV(USICLK) | /* Software clock strobe as source. */
           _BV(USITC);         /* Toggle Clock Port. */

    do {

        USICR = data;              /* Generate positive SCL edge. */
        while (!SCL_PIN_READ());   /* Wait for SCL to go high. */
        _delay_us(HIGH_PERIOD);
        USICR = data;
        _delay_us(LOW_PERIOD);     /* Generate negative SCL edge. */
  } while (!(USISR & 1<<USIOIF));  /* Check for transfer complete. */

  data = USIDR;              /* Read out data. */
  USIDR = 0xFF;
  SDA_PIN_HIGH();            /* Release SDA. */
  SDA_PIN_OUTPUT();          /* Enable SDA as output. */

  return data;
}