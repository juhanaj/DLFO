#ifndef DAC_H
#define DAC_H

#include <avr/io.h>
#include <stdint.h>

#include "spi.h"

#define DAC_PORT PORTD
#define DAC_DDR DDRD
#define DAC_CS PD5
#define DAC_LDAC PD6

class MCP4822 {
public:
  MCP4822() {}
  ~MCP4822() {}

  static const uint8_t DacOutA;
  static const uint8_t DacOutB;

  static const uint8_t OutputBit;
  static const uint8_t GainBit;
  static const uint8_t ChannelStateBit;

  void init() {
    Spi::init();

    // Set CS and LDAC pins as ouput.
    DAC_DDR |= (1 << DAC_CS) | (1 << DAC_LDAC);

    DAC_PORT |= (1 << DAC_CS) | (1 << DAC_LDAC);
  }

  void write(const uint16_t &out, const uint8_t &output = 0) {
    DAC_PORT |= (1 << DAC_LDAC);
    DAC_PORT &= ~(1 << DAC_CS);

    uint16_t data =
        (output << OutputBit) | (0 << GainBit) | (1 << ChannelStateBit) | out;
    Spi::write(data >> 8);   // First 8 bits
    Spi::write(data & 0xff); // Last 8 bits

    DAC_PORT |= (1 << DAC_CS);
    DAC_PORT &= ~(1 << DAC_LDAC);
  }
};

#endif
