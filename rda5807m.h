#ifndef __RDA5807M_H
#define __RDA5807M_H
#include "at85_i2c.h"
#include <stdbool.h>

// this chip only supports FM mode
#define FREQ_STEPS 10

#define RADIO_REG_CHIPID 0x00

// Register 2 bits
#define RADIO_REG_CTRL 0x02
#define RADIO_REG_CTRL_DHIZ 0x8000
#define RADIO_REG_CTRL_DMUTE 0x4000
#define RADIO_REG_CTRL_DMUTE_SHIFT 14
#define RADIO_REG_CTRL_MONO_SELECT 0x2000
#define RADIO_REG_CTRL_BASS_BOOST 0x1000
#define RADIO_REG_CLOCK_CAL      0x0800
#define RADIO_REG_RCLK_DIRECT    0x0400
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK 0x0100
#define RADIO_REG_SEEK_MODE   0x0080
#define RADIO_REG_CTRL_RDS 0x0008
#define RADIO_REG_CTRL_NEW 0x0004
#define RADIO_REG_CTRL_RESET 0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

// Register 3 bits
#define RADIO_REG_CHAN 0x03
#define RADIO_REG_CHANNEL   0xFFC0
#define RADIO_REG_DIRECT_MODE     0x0020
#define RADIO_REG_CHAN_TUNE 0x0010
#define RADIO_BAND_MASK 0x000C
#define RADIO_BAND_SHIFT 2
#define RADIO_CHAN_SHIFT 6
#define RADIO_REG_CHAN_BAND_US_EU 0x0 // [3:2] 00 87-108MHz(US/Europe)
#define RADIO_REG_CHAN_BAND_JP    0x4    // [3:2] 01 76-91MHz(Japan)
#define RADIO_REG_CHAN_BAND_WORLD 0x8 // [3:2] 10 76-108(world wide)
#define RADIO_REG_CHAN_BAND_EEUR  0xc // [3:2] 11 65 –76 MHz （East Europe） or 50-65MHz
#define RADIO_REG_SPACE_MASK 0x0003
#define RADIO_REG_CHAN_SPACE_00 0x0 // [1:0] 00 = 100kHz
#define RADIO_REG_CHAN_SPACE_01 0x1 // [1:0] 01 = 200kHz
#define RADIO_REG_CHAN_SPACE_10 0x2 // [1:0] 10 = 50kHz
#define RADIO_REG_CHAN_SPACE_11 0x3 // [1:0] 11 = 25kHz


#define RADIO_REG_R4 0x04
#define RADIO_REG_R4_EM50 0x0800
//      RADIO_REG_R4_RES   0x0400
#define RADIO_REG_R4_SOFTMUTE 0x0200
#define RADIO_REG_R4_AFC 0x0100

#define RADIO_REG_VOL 0x05
#define RADIO_REG_VOL_MASK 0x000F


#define RADIO_REG_I2S 0x06

#define RADIO_REG_BLEND 0x07
// Status bits
#define RADIO_REG_SOFTBLENDTH_MASK 0x7C00
#define RADIO_REG_SOFTBLENDTH_SHIFT 10
#define RADIO_REG_EASTBAND65M 0x0200
#define RADIO_REG_SOFTBLEND 0x1
#define RADIO_REG_FREQMODE  0x0


#define RADIO_REG_FREQ 0x08

#define RADIO_REG_RA 0x0A
// Status bits
#define RADIO_REG_RA_RDSR 0x8000
#define RADIO_REG_RA_STC 0x4000
#define RADIO_REG_RA_SF 0x2000
#define RADIO_REG_RA_RDSS 0x1000
#define RADIO_REG_RA_RDSBLOCK 0x0800
#define RADIO_REG_RA_STEREO 0x0400
#define RADIO_REG_RA_NR 0x03FF

#define RADIO_REG_RB 0x0B
#define RADIO_REG_RSSI_MASK 0xFE00
#define RADIO_REG_RSSI_SHIFT 9

#define RADIO_REG_RB_FMTRUE 0x0100
#define RADIO_REG_RB_FMREADY 0x0080

#define RADIO_REG_RDSA 0x0C
#define RADIO_REG_RDSB 0x0D
#define RADIO_REG_RDSC 0x0E
#define RADIO_REG_RDSD 0x0F

// I2C-Address RDA Chip for sequential  Access
#define I2C_SEQ 0x10

// I2C-Address RDA Chip for Index  Access
#define I2C_INDX 0x11

//Define RDA5807M I2C Addresses
#define RDA5807M_I2C_ADDR_SEQRDA (0x20 >> 1)
#define RDA5807M_I2C_ADDR_RANDOM (0x22 >> 1)
#define RDA5807M_I2C_ADDR_SEQTEA (0xC0 >> 1)


typedef enum {
    RADIO_BAND_EU = 0x00,      // 00 87-108MHz(US/Europe)
    RADIO_BAND_JP = 0x01,      // [3:2] 01 76-91MHz(Japan)
    RADIO_BAND_WD = 0x02,      // [3:2] 10 76-108(world wide)
    RADIO_BAND_EE = 0x03,      // [3:2] 11 65 –76 MHz （East Europe） or 50-65MHz
} RADIO_BAND;

typedef enum {
    CH_SPACE_100 = 0, // [1:0] 00 = 100kHz
    CH_SPACE_200, // [1:0] 01 = 200kHz
    CH_SPACE_50,  // [1:0] 10 = 50kHz
    CH_SPACE_25,  // [1:0] 11 = 25kHz
} CH_SPACE;

/// Frequency data type.
/// Only 16 bits are used for any frequency value (not the real one)
typedef uint16_t RADIO_FREQ;
void init_fm(void);
void set_volume(uint8_t newVolume);
void volume_up(void);
void volume_down(void);
uint8_t get_volume();
void set_bass_boost(bool switchOn);
void set_mono(bool switchOn);
void toggle_mute(void);
bool get_mute(void);
void toggle_power(void);
bool has_poweroff(void);

// ----- Receiver features -----
void set_band(RADIO_BAND newBand);
void set_ch_space(CH_SPACE newSpace);
uint16_t get_ch_space(void);

void shift_band(void);
uint8_t get_band(void);

void shift_space(void);
uint8_t get_space(void);

void set_frequency(uint16_t frequency);
RADIO_FREQ get_frequency(void);
uint8_t get_rssi(void);

void seek_up();   // start seek mode upwards
void seek_down(); // start seek mode downwards

// ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD

uint16_t _reg_mem[16];
RADIO_BAND _band; // Last set band.
RADIO_FREQ _freq; // Last set frequency.
CH_SPACE  _space; // Last set space.

RADIO_FREQ _freqLow;   // Lowest frequency of the current selected band.
RADIO_FREQ _freqHigh;  // Highest frequency of the current selected band.
uint8_t _freqSteps; // Resolution of the tuner.
void _write_register(uint8_t reg, uint16_t value);
void _save_registers(void);
uint16_t _read_register(uint8_t reg);

#endif