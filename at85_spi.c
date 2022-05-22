/********************************************************************************
 * nrf24l01-ble/at85_spi.c
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

#include "at85_spi.h"

void spi_setup()
{

  /* set PB1,PB2,PB3 to be output */

  DDRB  = _BV(SPI_MISO) |_BV(SPI_SCK) | _BV(SPI_CSN); /* 0b00001110 */
  USICR = _BV(USIWM0);          /* Choosing SPI aka three wire mode, Mode */

  /* make SCL low to mode 0 (CPOL=0) */
  PORTB &= ~_BV(SPI_SCK);
  PORTB &= ~_BV(SPI_MISO);
}

void spi_dosend(uint8_t cmd,uint8_t *buf, uint8_t len)
{
  CSN_LOW();
  spi_write(cmd);
  while(len--)
  {
    spi_write(*buf++);
  }
  CSN_HIGH();
}

void spi_onecmd(uint8_t cmd)
{
  CSN_LOW();
  spi_write(cmd);
  CSN_HIGH();
}

void spi_readreg(uint8_t reg,uint8_t *buf,uint8_t len)
{
  while(len--)
  {
    CSN_LOW();
    spi_write(reg);
    while(USIOIF == 1)
    {
      *buf++ = USIBR;  /* Reading received byte from USIBR buffer register */
      USISR |= (1 << USIOIF);
    }
    CSN_HIGH();
  }

}

void spi_write(uint8_t byte)
{
  USIDR = byte;

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USICR |= (1 << USITC);
  USICR |= (1 << USITC)|(1 << USICLK);

  USISR  = 0 ;
}

void spi_sendcmd(uint8_t cmd, uint8_t data)
{
    CSN_LOW();
    spi_write(cmd);
    spi_write(data);
    CSN_HIGH();
}

void spi_sendblock(uint8_t *buf,uint8_t length)
{
    CSN_LOW();
    while(length--)
    {
      spi_write(*buf++);
    }
    CSN_HIGH();
}
