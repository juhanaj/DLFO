#ifndef DAC_H
#define DAC_H

#include <stdint.h>
#include <avr/io.h>

#include "spi.h"

#define DAC_PORT PORTD
#define DAC_DDR DDRD
#define DAC_CS PD5
#define DAC_LDAC PD6

const uint8_t DAC_OUTPUT = 15;
const uint8_t DAC_GAIN = 13;
const uint8_t DAC_CHANNEL = 12;

class MCP4822 {
    public:
        MCP4822() {}
        ~MCP4822() {}

    void init() {
        Spi::init();

        // Set CS and LDAC pins as ouput.
        DAC_DDR |= (1<<DAC_CS) | (1<<DAC_LDAC);

        DAC_PORT |= (1<<DAC_CS) | (1<<DAC_LDAC);
    }

    void write(uint16_t out) {
        DAC_PORT |= (1<<DAC_LDAC);
        DAC_PORT &= ~(1<<DAC_CS);

        uint16_t data = (0<<DAC_OUTPUT) | (0<<DAC_GAIN) | (1<<DAC_CHANNEL) | out;
        Spi::write(data>>8); // First 8 bits
        Spi::write(data&0xff); // Last 8 bits

        DAC_PORT |= (1<<DAC_CS);
        DAC_PORT &= ~(1<<DAC_LDAC);
    }
};

#endif
