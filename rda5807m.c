#include "rda5807m.h"
// https://www.hackster.io/indoorgeek/fm-radio-using-arduino-and-rda8057m-73a262
// https://tomeko.net/projects/RDA5807M_radio/index.php?lang=en
// https://github.com/f5swb/RDA5807
// https://github.com/mathertel/Radio

void init_fm(void) {
    _reg_mem[RADIO_REG_CHIPID] = 0x5807;
    _write_register(RADIO_REG_CHIPID, _reg_mem[RADIO_REG_CHIPID]);
    _reg_mem[RADIO_REG_CTRL] = (RADIO_REG_CTRL_RESET | RADIO_REG_CTRL_ENABLE);
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    set_band(RADIO_BAND_FM);
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

void set_band(RADIO_BAND newBand) {
    _band = newBand == RADIO_BAND_FM ? RADIO_REG_CHAN_BAND_FM : RADIO_REG_CHAN_BAND_FMWORLD;
    _band |= RADIO_REG_CHAN_SPACE_100;
    _reg_mem[RADIO_REG_CHAN] = _band;
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);
}

void set_frequency(RADIO_FREQ newF) {
    uint16_t newChannel;
    uint16_t regChannel = _reg_mem[RADIO_REG_CHAN] & (RADIO_REG_CHAN_SPACE | RADIO_REG_CHAN_BAND);

    if (newF < _freqLow)
        newF = _freqLow;
    if (newF > _freqHigh)
        newF = _freqHigh;
    newChannel = (newF - _freqLow) / 10;

    regChannel += RADIO_REG_CHAN_TUNE; // enable tuning
    regChannel |= newChannel << 6;

    // enable output and unmute
    _reg_mem[RADIO_REG_CTRL] = RADIO_REG_CTRL_OUTPUT | RADIO_REG_CTRL_UNMUTE | RADIO_REG_CTRL_RDS | RADIO_REG_CTRL_ENABLE;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _reg_mem[RADIO_REG_CHAN] = regChannel;
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);

    // adjust Volume
    _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
}

void seek_up(bool toNextSender) {
    _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEKUP | RADIO_REG_CTRL_SEEK;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_SEEK; // clear seekmode;

    if (!toNextSender) {
        // stop scanning right now
        _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    }
}

void seek_down(bool toNextSender) {
    uint16_t val = RADIO_REG_CTRL_SEEK;
    _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_SEEKUP;
    _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_SEEK; // clear seekmode;

    if (!toNextSender) {
        // stop scanning right now
        _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    }
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
        _reg_mem[RADIO_REG_CTRL] |= (RADIO_REG_CTRL_MONO);
    } else {
        _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_MONO;
    }
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_bass_boost(bool switchOn)
{
    if (switchOn) {
        _reg_mem[RADIO_REG_CTRL] |= (RADIO_REG_CTRL_BASS);
    } else {
        _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_BASS;
    }
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void set_volume(uint8_t newVolume)
{
    newVolume &= RADIO_REG_VOL_VOL;
    _reg_mem[RADIO_REG_VOL] &= (~RADIO_REG_VOL_VOL);
    _reg_mem[RADIO_REG_VOL] |= newVolume;
    _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
}

void _write_register(uint8_t reg, uint16_t value) {
    i2c_start(I2C_INDX, USI_WRITE);
    i2c_write(0);
    i2c_write(reg);
    i2c_write(value >> 8);
    i2c_write(value & 0xff);
    i2c_stop();
}

uint16_t _read_register(uint8_t reg) {
    i2c_start(I2C_SEQ, USI_WRITE);
    uint8_t hb = i2c_read(0);
    uint8_t lb = i2c_read(0);
    return ((hb << 8) & lb);
}