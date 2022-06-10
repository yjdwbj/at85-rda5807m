#include "rda5807m.h"
#include <avr/interrupt.h>

// https://www.hackster.io/indoorgeek/fm-radio-using-arduino-and-rda8057m-73a262
// https://tomeko.net/projects/RDA5807M_radio/index.php?lang=en
// https://github.com/f5swb/RDA5807
// https://github.com/mathertel/Radio
// https://circuitdigest.com/microcontroller-projects/arduino-fm-radio-using-rda5807
// https://github.com/rmrfus/tinybme

void init_fm(void) {
    _reg_mem[RADIO_REG_CHIPID] = 0x5807;
    _write_register(RADIO_REG_CHIPID, _reg_mem[RADIO_REG_CHIPID]);
    _reg_mem[RADIO_REG_CTRL] = (RADIO_REG_CTRL_RESET | RADIO_REG_CTRL_ENABLE);
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    set_ch_space(CH_SPACE_100);
    set_band(RADIO_BAND_WD);

    //  0x1800;  // 04 DE ? SOFTMUTE
    _reg_mem[RADIO_REG_R4] = RADIO_REG_R4_EM50;
    _write_register(RADIO_REG_R4, _reg_mem[RADIO_REG_R4]);
    // 0x81D1;  // 0x82D1 / INT_MODE, SEEKTH=0110,????, Volume=1
    _reg_mem[RADIO_REG_VOL] = 0x9081;
    _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
    // reset the chip
    _reg_mem[RADIO_REG_CTRL] = RADIO_REG_CTRL_ENABLE;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_ch_space(CH_SPACE newSpace)
{
    _reg_mem[RADIO_REG_CHAN] &= ~0x3;
    _space = newSpace;
    _reg_mem[RADIO_REG_CHAN] |= _space;
    switch (newSpace)
    {
    case CH_SPACE_25:
        _freqSteps = 2.5;
        break;
    case CH_SPACE_50:
        _freqSteps = 5;
        break;
    case CH_SPACE_100:
        _freqSteps = 10;
        break;
    case CH_SPACE_200:
        _freqSteps = 20;
        break;
    default:
        break;
    }
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);
}

uint8_t get_ch_space(void)
{
    return _freqSteps;
}

void set_band(RADIO_BAND newBand) {
    _band = newBand;
    _reg_mem[RADIO_REG_CHAN] &= ~0xc;
    switch (newBand)
    {
    case RADIO_BAND_EU:
        _freqLow = 8700;
        _freqHigh = 10800;
        _reg_mem[RADIO_REG_CHAN] |= 0x00;
        break;
    case RADIO_BAND_JP:
        _freqLow = 7600;
        _freqHigh = 9100;
        _reg_mem[RADIO_REG_CHAN] |= 0x4;
        break;
    case RADIO_BAND_WD:
        _freqLow = 7600;
        _freqHigh = 10800;
        _reg_mem[RADIO_REG_CHAN] |= 0x8;
        break;
    case RADIO_BAND_EE:
        _freqLow = 6500;
        _freqHigh = 7600;
        _reg_mem[RADIO_REG_CHAN] |= 0xc;
        break;
    default:
        break;
    }
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);
}

void set_frequency(RADIO_FREQ newF) {
    uint16_t newChannel;
    uint16_t regChannel = _reg_mem[RADIO_REG_CHAN];

    if (newF < _freqLow)
        newF = _freqLow;
    if (newF > _freqHigh)
        newF = _freqHigh;
    newChannel = (newF - _freqLow) / _freqSteps;

    regChannel += RADIO_REG_CHAN_TUNE; // enable tuning
    regChannel |= newChannel << 6;

    // enable output and unmute
    _reg_mem[RADIO_REG_CTRL] = RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE | RADIO_REG_CTRL_RDS | RADIO_REG_CTRL_ENABLE;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _reg_mem[RADIO_REG_CHAN] = regChannel;
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);

    // adjust Volume
    // _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
}

RADIO_FREQ get_frequency(void) {
    _reg_mem[RADIO_REG_RA] = (_read_register() & RADIO_REG_RA_NR) / 10;
    _freq = _freqSteps  * _reg_mem[RADIO_REG_RA] + _freqLow;
    return _freq;
}

void seek_up(bool toNextSender) {
    _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEKUP | RADIO_REG_CTRL_SEEK;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void seek_down(bool toNextSender) {
    _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_SEEKUP;
    _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_soft_mute(bool switchOn) {
    if (switchOn) {
        _reg_mem[RADIO_REG_R4] |= RADIO_REG_R4_SOFTMUTE;
    } else {
        _reg_mem[RADIO_REG_R4] &= (~RADIO_REG_R4_SOFTMUTE);
    }
    _write_register(RADIO_REG_R4, _reg_mem[RADIO_REG_R4]);
}

void set_mute(bool switchOn) {
    if (switchOn) {
        _reg_mem[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_UNMUTE);
    } else {
        _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_UNMUTE;
    }
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_mono(bool switchOn) {
    _reg_mem[RADIO_REG_CTRL] &= (~RADIO_REG_CTRL_SEEK);
    if (switchOn) {
        _reg_mem[RADIO_REG_CTRL] |= (RADIO_REG_CTRL_MONO_SELECT);
    } else {
        _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_MONO_SELECT;
    }
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_bass_boost(bool switchOn) {
    if (switchOn) {
        _reg_mem[RADIO_REG_CTRL] |= (RADIO_REG_CTRL_BASS_BOOST);
    } else {
        _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_BASS_BOOST;
    }
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_volume(uint8_t newVolume) {
    _reg_mem[RADIO_REG_VOL] = (_reg_mem[RADIO_REG_VOL] & 0xfff0 ) | (newVolume & 0x0f);
    _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
}

uint8_t get_volume() {
    return _reg_mem[RADIO_REG_VOL] & RADIO_REG_VOL_VOL;
}

void _write_register(uint8_t reg, uint16_t value) {
    i2c_start(I2C_INDX, USI_WRITE);
    i2c_write(reg);
    i2c_write(value >> 8);
    i2c_write(value & 0xff);
    i2c_stop();
}

uint16_t _read_register() {
    i2c_start(I2C_INDX, USI_READ);
    uint8_t hb = i2c_read();
    uint8_t lb = i2c_read();
    i2c_stop();
    return ((hb << 8) + lb);
}