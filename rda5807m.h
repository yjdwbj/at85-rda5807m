#ifndef __RDA5807M_H
#define __RDA5807M_H
#include "at85_i2c.h"
#include <stdbool.h>

// this chip only supports FM mode
#define FREQ_STEPS 10

#define RADIO_REG_CHIPID 0x00

#define RADIO_REG_CTRL 0x02
#define RADIO_REG_CTRL_OUTPUT 0x8000
#define RADIO_REG_CTRL_UNMUTE 0x4000
#define RADIO_REG_CTRL_MONO 0x2000
#define RADIO_REG_CTRL_BASS 0x1000
#define RADIO_REG_CTRL_SEEKUP 0x0200
#define RADIO_REG_CTRL_SEEK 0x0100
#define RADIO_REG_CTRL_RDS 0x0008
#define RADIO_REG_CTRL_NEW 0x0004
#define RADIO_REG_CTRL_RESET 0x0002
#define RADIO_REG_CTRL_ENABLE 0x0001

#define RADIO_REG_CHAN 0x03
#define RADIO_REG_CHAN_SPACE 0x0003
#define RADIO_REG_CHAN_SPACE_100 0x0000
#define RADIO_REG_CHAN_BAND 0x000C
#define RADIO_REG_CHAN_BAND_FM 0x0000
#define RADIO_REG_CHAN_BAND_FMWORLD 0x0008
#define RADIO_REG_CHAN_TUNE 0x0010
//      RADIO_REG_CHAN_TEST   0x0020
#define RADIO_REG_CHAN_NR 0x7FC0

#define RADIO_REG_R4 0x04
#define RADIO_REG_R4_EM50 0x0800
//      RADIO_REG_R4_RES   0x0400
#define RADIO_REG_R4_SOFTMUTE 0x0200
#define RADIO_REG_R4_AFC 0x0100

#define RADIO_REG_VOL 0x05
#define RADIO_REG_VOL_VOL 0x000F

#define RADIO_REG_RA 0x0A
#define RADIO_REG_RA_RDS 0x8000
#define RADIO_REG_RA_RDSBLOCK 0x0800
#define RADIO_REG_RA_STEREO 0x0400
#define RADIO_REG_RA_NR 0x03FF

#define RADIO_REG_RB 0x0B
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

typedef enum {
    RADIO_BAND_NONE = 0, ///< No band selected.

    RADIO_BAND_FM = 0x01,      ///< FM band 87.5 - 108 MHz (USA, Europe) selected.
    RADIO_BAND_FMWORLD = 0x02, ///< FM band 76 - 108 MHz (Japan, Worldwide) selected.
    RADIO_BAND_AM = 0x03,      ///< AM band selected.
    RADIO_BAND_KW = 0x04,      ///< KW band selected.

    RADIO_BAND_FMTX = 0x11, ///< Transmit for FM.
} RADIO_BAND;

/// Frequency data type.
/// Only 16 bits are used for any frequency value (not the real one)
typedef uint16_t RADIO_FREQ;
void init_fm(void);
void set_volume(uint8_t newVolume);
void set_bass_boost(bool switchOn);
void set_mono(bool switchOn);
void set_mute(bool switchOn);
void set_soft_mute(bool switchOn); ///< Set the soft mute mode (mute on low signals) on or off.

// ----- Receiver features -----
void set_band(RADIO_BAND newBand);
void set_frequency(RADIO_FREQ newF);
RADIO_FREQ get_frequency(void);

void seek_up(bool toNextSender);   // start seek mode upwards
void seek_down(bool toNextSender); // start seek mode downwards

// ----- Supporting RDS for RADIO_BAND_FM and RADIO_BAND_FMWORLD

void checkRDS();

uint16_t _reg_mem[16];
RADIO_BAND _band; ///< Last set band.
RADIO_FREQ _freq; ///< Last set frequency.

RADIO_FREQ _freqLow;   ///< Lowest frequency of the current selected band.
RADIO_FREQ _freqHigh;  ///< Highest frequency of the current selected band.
RADIO_FREQ _freqSteps; ///< Resolution of the tuner.
void _write_register(uint8_t reg, uint16_t value);
void _save_registers(void);
uint16_t _read_register(uint8_t reg);

#endif