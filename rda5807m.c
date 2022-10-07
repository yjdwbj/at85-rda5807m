#include "rda5807m.h"
#include <avr/interrupt.h>

// https://www.hackster.io/indoorgeek/fm-radio-using-arduino-and-rda8057m-73a262
// https://tomeko.net/projects/RDA5807M_radio/index.php?lang=en
// https://github.com/f5swb/RDA5807
// https://github.com/mathertel/Radio
// https://circuitdigest.com/microcontroller-projects/arduino-fm-radio-using-rda5807
// https://github.com/rmrfus/tinybme
// https://funprojects.blog/tag/rda5807/
// https://github.com/csdexter/RDA5807M

void init_fm(void) {
    _reg_mem[RADIO_REG_CHIPID] = 0x5807;
    _write_register(RADIO_REG_CHIPID, _reg_mem[RADIO_REG_CHIPID]);
    _reg_mem[RADIO_REG_CTRL] = (RADIO_REG_CTRL_DHIZ |
                                RADIO_REG_CTRL_BASS_BOOST |
                                RADIO_REG_CTRL_DMUTE |
                                RADIO_REG_CTRL_RESET |
                                RADIO_REG_CTRL_ENABLE |
                                RADIO_REG_CTRL_NEW |
                                RADIO_REG_CTRL_RDS);
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    set_ch_space(CH_SPACE_100); // RDA5807m can't work on 50k space.
    set_band(RADIO_BAND_WD);

    //  0x1800;  // 04 DE ? SOFTMUTE
    // _reg_mem[RADIO_REG_R4] |= RADIO_REG_R4_EM50 | (1 << 11);
    // _write_register(RADIO_REG_R4, _reg_mem[RADIO_REG_R4]);
    _delay_ms(40);
}

void set_ch_space(CH_SPACE newSpace) {
    _reg_mem[RADIO_REG_CHAN] &= ~0x3;
    _space = newSpace;
    _reg_mem[RADIO_REG_CHAN] |= _space;
    switch (newSpace) {
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

void set_tune(bool flag) {
    _reg_mem[RADIO_REG_CHAN] = _read_register(RADIO_REG_CHAN);
    bool en = ((_reg_mem[RADIO_REG_CHAN] & _BV(RADIO_REG_TUNE_SHIFT)) >> RADIO_REG_TUNE_SHIFT) == 1;
    if (flag && en || !flag && !en)
        return;
    if (flag)
        _reg_mem[RADIO_REG_CHAN] |= _BV(RADIO_REG_TUNE_SHIFT);
    else
        _reg_mem[RADIO_REG_CHAN] &= ~_BV(RADIO_REG_TUNE_SHIFT);
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);
}

void set_band(RADIO_BAND newBand) {
    _reg_mem[RADIO_REG_CHAN] &= ~RADIO_BAND_MASK;
    uint8_t space = _reg_mem[RADIO_REG_CHAN] & RADIO_REG_SPACE_MASK;
    switch (newBand) {
    case RADIO_BAND_EU:
        _freqLow = 8700;
        _freqHigh = 10800;
        break;
    case RADIO_BAND_JP:
    case RADIO_BAND_WD:
        _freqLow = 7600;
        _freqHigh = 10800;
        break;
    case RADIO_BAND_EE:
        _freqLow = 6500;
        _freqHigh = 7600;
        break;
    default:
        break;
    }
    _reg_mem[RADIO_REG_CHAN] |= (newBand << RADIO_BAND_SHIFT) | space;
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);
}

void shift_band(void) {
    uint8_t band = get_band();
    if (band == RADIO_BAND_EE)
        band = RADIO_BAND_EU;
    else
        band += 1;
    set_band(band);
    seek_up();
}

uint8_t get_band(void) {
    _reg_mem[RADIO_REG_CHAN] = _read_register(RADIO_REG_CHAN);
    return (uint8_t)((_reg_mem[RADIO_REG_CHAN] & RADIO_BAND_MASK) >> RADIO_BAND_SHIFT);
}

void shift_space(void) {
    uint8_t space = _read_register(RADIO_REG_CHAN) & RADIO_REG_SPACE_MASK;
    if (space == CH_SPACE_25)
        space = RADIO_BAND_EU;
    else
        space += 1;
    set_ch_space(space);
    seek_up();
}

uint8_t get_space(void) {
    return (uint8_t)(_read_register(RADIO_REG_CHAN) & RADIO_REG_SPACE_MASK);
}

uint16_t get_ch_space(void) {
    uint8_t band = _read_register(RADIO_REG_CHAN) & (RADIO_BAND_MASK | RADIO_REG_SPACE_MASK);
    const uint8_t space = band & RADIO_REG_SPACE_MASK;

    if (band & RADIO_BAND_MASK == RADIO_REG_CHAN_BAND_EEUR &&
        !(_read_register(RADIO_REG_BLEND) & RADIO_REG_EASTBAND65M))
        // Lower band limit is 50MHz
        band = (band >> RADIO_BAND_SHIFT) + 1;
    else
        band >>= RADIO_BAND_SHIFT;
    return (space << 8 | band);
}

void set_frequency(uint16_t newF) {
    uint16_t newChannel;
    uint16_t regChannel = 0x0;

    if (newF < _freqLow)
        newF = _freqLow;
    if (newF > _freqHigh)
        newF = _freqHigh;
    newChannel = (newF - _freqLow) / _freqSteps;

    regChannel |= RADIO_REG_CHAN_TUNE; // enable tuning
    regChannel |= newChannel << RADIO_CHAN_SHIFT;

    //  enable output and unmute
    _reg_mem[RADIO_REG_CTRL] = RADIO_REG_CTRL_DHIZ |
                                RADIO_REG_CTRL_DMUTE |
                                RADIO_REG_CTRL_NEW |
                                RADIO_REG_CTRL_RDS |
                                RADIO_REG_CTRL_ENABLE;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _reg_mem[RADIO_REG_CHAN] |= regChannel;
    _write_register(RADIO_REG_CHAN, _reg_mem[RADIO_REG_CHAN]);
}

RADIO_FREQ get_frequency(void) {
    _delay_ms(40); // wait for seek done.
    // do {
    _reg_mem[RADIO_REG_RA] = _read_register(RADIO_REG_RA);
    // } while (((_reg_mem[RADIO_REG_RA] & RADIO_REG_RA_STC) >> RADIO_REG_RA_STC_SHIFT) == 0);
    return _freqLow +
           (_reg_mem[RADIO_REG_RA] & RADIO_REG_RA_NR) * _freqSteps;
}

uint8_t get_rssi(void) {
    _delay_ms(40);
    _reg_mem[RADIO_REG_RB] = _read_register(RADIO_REG_RB);
    const uint8_t rssi = (_reg_mem[RADIO_REG_RB] & RADIO_REG_RSSI_MASK) >> RADIO_REG_RSSI_SHIFT;
    return rssi;
}

void seek_up() {
    _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEKUP | RADIO_REG_CTRL_SEEK;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void seek_down() {
    _reg_mem[RADIO_REG_CTRL] &= ~(RADIO_REG_CTRL_SEEKUP | RADIO_REG_CTRL_SEEK | RADIO_REG_SEEK_MODE);
    _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_SEEK;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
}

void toggle_mute() {
    bool mute = get_mute();
    if (mute)
        _reg_mem[RADIO_REG_CTRL] |= RADIO_REG_CTRL_DMUTE;
    else
        _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_DMUTE;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _delay_ms(40);
}

bool get_mute() {
    _reg_mem[RADIO_REG_CTRL] = _read_register(RADIO_REG_CTRL);
    bool mute = (_reg_mem[RADIO_REG_CTRL] & RADIO_REG_CTRL_DMUTE) >> RADIO_REG_CTRL_DMUTE_SHIFT == 0;
    return mute;
}

void toggle_power(void) {
    bool poweroff = has_poweroff();
    // static RADIO_FREQ ch = get_frequency();
    if (poweroff) {
        set_frequency(get_frequency());
        return;
    }
    _reg_mem[RADIO_REG_CTRL] &= ~RADIO_REG_CTRL_ENABLE;
    _write_register(RADIO_REG_CTRL, _reg_mem[RADIO_REG_CTRL]);
    _delay_ms(40);
}

bool has_poweroff(void) {
    _reg_mem[RADIO_REG_CTRL] = _read_register(RADIO_REG_CTRL);
    return (_reg_mem[RADIO_REG_CTRL] & RADIO_REG_CTRL_ENABLE) == 0;
}

void set_volume(uint8_t newVolume) {
    _reg_mem[RADIO_REG_VOL] &= ~RADIO_REG_VOL_MASK;
    _reg_mem[RADIO_REG_VOL] |= newVolume;
    _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
}

void volume_up(void) {
    const uint8_t volume = get_volume();
    if (volume != RADIO_REG_VOL_MASK) {
        _reg_mem[RADIO_REG_VOL] &= ~RADIO_REG_VOL_MASK;
        _reg_mem[RADIO_REG_VOL] |= (volume + 1);
        _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
    }
}

void volume_down(void) {
    const uint8_t volume = get_volume();
    if (volume) {
        _reg_mem[RADIO_REG_VOL] &= ~RADIO_REG_VOL_MASK;
        _reg_mem[RADIO_REG_VOL] |= (volume - 1);
        _write_register(RADIO_REG_VOL, _reg_mem[RADIO_REG_VOL]);
    }
}

uint8_t get_volume() {
    _reg_mem[RADIO_REG_VOL] = _read_register(RADIO_REG_VOL);
    return (uint8_t)(_reg_mem[RADIO_REG_VOL] & RADIO_REG_VOL_MASK);
}

void _write_register(uint8_t reg, uint16_t value) {
    i2c_start(I2C_INDX, USI_WRITE);
    i2c_write(reg);
    i2c_write(value >> 8);
    i2c_write(value & 0x00ff);
    i2c_stop();
}

uint16_t _read_register(uint8_t reg) {
    uint16_t data;
    i2c_start(RDA5807M_I2C_ADDR_RANDOM, USI_WRITE);
    i2c_write(reg);
    i2c_start(RDA5807M_I2C_ADDR_RANDOM, USI_READ);
    data = i2c_read16();
    i2c_stop();
    return data;
}
