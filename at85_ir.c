#include "at85_ir.h"
#include "oled.h"

// https://exploreembedded.com/wiki/NEC_IR_Remote_Control_Interface_with_8051
// https://techdocs.altium.com/display/FPGA/NEC+Infrared+Transmission+Protocol
// https://circuitdigest.com/microcontroller-projects/build-your-own-ir-remote-decoder-using-tsop-and-pic-microcontroller

static volatile bool repeatCode = false;

// 4Byte of IR data,
// LSB first, 1 start bit + 16 bit address (or 8 bit address and 8 bit inverted address) + 8 bit command + 8 bit inverted command + 1 stop bit.
#define BUFFER_SIZE 32

// Pulse buffer index (changes from 0 to BUFFER_SIZE)
static volatile uint8_t bufferIndex = 0;

// Current state of capturing
static volatile IRCaptureState captureState = WAIT_STATE;

#define LCD_BUFFER_SIZE 16
uint8_t lcd_buffer[LCD_BUFFER_SIZE];
uint8_t irdata[4] = {0, 0, 0, 0};
IR_data *IRData = irdata;

/**
 * Table 10-1. Reset and Interrupt Vectors
 *
  Vec No. |Program Addr|  Source    | Interrupt Definition
  --------+------------+------------+-----------------------------
    1     |   0x0000   |   RESET    | External pin, power-on reset, brown-out reset,watchdog reset
    2     |   0x0001   |   INT0     | External interrupt request 0
    3     |   0x0002   |   PCINT0   | Pin change interrupt request 0
    4     |   0x0003   |   PCINT1   | Pin change interrupt request 1
    5     |   0x0004   |   WDT      | Watchdog time-out
    6     |   0x0005   |   TIMER1   | CAPT Timer/Counter1 capture event
    7     |   0x0006   |   TIMER1   | COMPA Timer/Counter1 compare match A
    8     |   0x0007   |   TIMER1   | COMPB Timer/Counter1 compare match B
    9     |   0x0008   |   TIMER1   | OVF Timer/Counter0 overflow
    10    |   0x0009   |   TIMER0   | COMPA Timer/Counter0 compare match A
    11    |   0x000A   |   TIMER0   | COMPB Timer/Counter0 compare match B
    12    |   0x000B   |   TIMER0   | OVF Timer/Counter0 overflow
    13    |   0x000C   |   ANA_COMP | Analog comparator
    14    |   0x000D   |   ADC ADC  | conversion complete
    15    |   0x000E   |   EE_RDY   | EEPROM ready
    16    |   0x000F   |   USI_START| USI START
    17    |   0x0010   |   USI_OVF  | USI overflow
*/

void ir_bus_init(void) {
    // DDRB &= ~_BV(PB2); //   IR data pin ,set input
    TCCR0A = 0;
    TCCR0B = 0;
    MCUCR |= _BV(ISC01); // The falling edge of INT0 generates an interrupt request.
    GIMSK |= _BV(INT0);  // External Interrupt Request 0, must give +5v power.
}

static uint8_t check_low_time(bool isLeading) {
    unsigned char val;
    TCCR0A = 0;
    TCNT0 = 0;
    // if isLeading using 1024 prescaler, one TICK is 0.000128s,MAX=32.768ms
    // otherwise using 64 prescaler, one TICK is 0.0000008s, MAX=2048us,
    TCCR0B |= (isLeading ? _BV(CS02) : _BV(CS01)) | _BV(CS00);
    TIMSK |= _BV(TOIE0);
    while (!(PINB & _BV(PB2)))
        ;
    TCCR0B = 0;
    val = TCNT0;
    return val;
}

static uint8_t check_high_time(bool isLeading) {
    unsigned char val;
    TCCR0A = 0;
    TCNT0 = 0;
    // https://www.arduinoslovakia.eu/application/timer-calculator
    // The CPU clock frequenct is 8MHz, so time period T = 1/8M = 0.000125ms.
    // a 16-bit timer(MAX = 65535),and 8-bit timer (MAX=255).
    TCCR0B |= (isLeading ? _BV(CS02) : _BV(CS01)) | _BV(CS00);
    TIMSK |= _BV(TOIE0);
    while ((PINB & _BV(PB2)))
        ;
    TCCR0B = 0;
    val = TCNT0;
    return val;
}

ISR(TIMER0_OVF_vect) {
}

ISR(INT0_vect) {
    uint8_t countNum = 0;
    switch (captureState) {
    // Waiting for next data
    case WAIT_STATE:
        countNum = check_low_time(true);
        // check it is valid 9ms leading pulse burst,pulse tolerance greater than 8.5ms and less than 9.6ms.
        // 67 * 128us = 8.576ms, 75 * 128us = 9.6ms
        if (countNum < 67 || countNum > 75)
            break;
        countNum = check_high_time(true);
        if (countNum > 16 && countNum < 22) {
            // check it's repeat codes leading valid 2.25ms space
            captureState = FINAL_PULSE_STATE;
            repeatCode = true;
        } else if (countNum >= 32 && countNum <= 36) {
            // check it's valid 4.5ms space, 32 * 128us = 4.096ms, 36 * 128us = 4.608ms
            captureState = DATA_STATE;
            bufferIndex = 0;
        }
        break;
    // Initial space or repeat space
    case FINAL_PULSE_STATE:
        countNum = check_low_time(false);
        // a 562.5µs pulse burst to mark the end of the space (and hence end of the transmitted repeat code).
        // 55 * 8us = 445us, 81 * 8us = 648us
        if (countNum < 55 || countNum > 81) {
            bufferState = BUF_NOT_READY;
            break;
        }
        bufferState = repeatCode ? BUF_REPEAT : BUF_READY;
        break;
    // 8-bit address
    case DATA_STATE:
        countNum = check_low_time(false);
        if (countNum < 55 || countNum > 81) {
            bufferState = BUF_NOT_READY;
            break;
        }
        countNum = check_high_time(false);
        if (countNum < 55 || countNum > 254) {
            bufferState = BUF_NOT_READY;
            break;
        }
        char index = bufferIndex / 8;
        irdata[index] >>= 1;
        // Logical '0' – a 562.5µs pulse burst followed by a 562.5µs space, with a total transmit time of 1.125ms
        // Logical '1' – a 562.5µs pulse burst followed by a 1.6875ms space, with a total transmit time of 2.25ms
        if (countNum > 140)
            irdata[index] |= 0x80; // 140 * 8us = 1.12ms, greater than 1.12ms means logical '1'.
        if (bufferIndex++ >= BUFFER_SIZE) {
            captureState = FINAL_PULSE_STATE;
        }
        break;
    }

    TCNT0 = 0;
}

bool ir_data_ready(void) {
    if (bufferState == BUF_READY) {
        cli(); // Disable INT0, prepare to LCD show string.
        oled_clear();
        sprintf(lcd_buffer, "0x%x", IRData->cmd);
        oled_p8x16str(0, 4, lcd_buffer);
        memset(lcd_buffer, 0, LCD_BUFFER_SIZE);
        switch (IRData->cmd) {
        case 0xfa05:
            seek_up(true);
            break;
        case 0xfa02:
            seek_down(true);
            break;
        case 0xe11e:
            set_volume(get_volume() + 1);
            break;
        case 0xf50a:
            set_volume(get_volume() - 1);
            break;
        default:
            break;
        }
        sei();
        bufferIndex = 0;
        bufferState = BUF_NOT_READY;
        captureState = WAIT_STATE;
        repeatCode = false;
    }
}
