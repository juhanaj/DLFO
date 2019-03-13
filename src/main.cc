#include <util/delay.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "mcp4822.h"

extern "C" {
#include "uart.h"
}

#include "wavetable.h"

// Macro for calculating array size
#define ARRAY_SIZE(x) sizeof(x) / sizeof(x[0])

MCP4822 dac;

const static uint32_t SHIFT_AMOUNT = 16;
const static uint32_t SHIFT_MASK = (((uint32_t)1 << SHIFT_AMOUNT) - 1);
const static uint32_t MAX_FREQ = 13107200;      // ~200Hz bit shifted left by 16
const static uint32_t MIN_FREQ = 2000;          // ~0.03Hz bit shifted left by 16
const static uint32_t FREQ_POT_SCALE = 12800;   // ~0.2Hz
const static uint32_t SAMPLE_FREQ = 11363;      // Sample write frequency to DAC

volatile uint32_t phase_delta = 0;
volatile uint32_t phase = 0;

int main(void) {
    cli();

    // Setup audio out timer
    TCCR2A = 0;
    TCCR2B = 0;
    TCNT2 = 0;
    TCCR2A = (1 << WGM21);
    TCCR2B = (1 << CS22);

    OCR2A = 21;
    TIMSK2 = (1 << OCIE2A);

    dac.init();

    // Enabled USART and redirect standard input/output to UART
    Uart::init();
    stdout = uart_output;
    stdin = uart_input;

    sei(); // turn on interrupts

    // Set up analog read parameters for reading potentiometer
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Reference AVCC
    ADMUX &= ~((1 << MUX0) | (1 << MUX1) | (1 << MUX2) | (1 << MUX3));
    ADMUX |= (1 << REFS0);

    // Free running mode
    ADCSRA |= (1 << ADSC);

    // Enable ADC
    ADCSRA |= (1 << ADEN);

    // Setup reset pin as input
    DDRD |= (1 << PD7);

    while(1) {
      // Start ADC conversion
      ADCSRA |= (1 << ADSC);
      while(ADCSRA & (1<<ADSC));
      uint32_t freq_pot = (ADCL | (ADCH << 8));

      // Scale potentiometer values
      uint32_t freq = freq_pot * FREQ_POT_SCALE;

      // Calculate phase increment from SAMPLE_FREQ, array size and freq
      phase_delta = ((ARRAY_SIZE(wavetable) * (freq / (SAMPLE_FREQ << SHIFT_AMOUNT))));

      // Reset phase if reset pin is high
      if (PIND & PD7)
        phase = 0;

      _delay_ms(2);
    }
}



ISR(TIMER2_COMPA_vect) {
  phase += phase_delta;

  unsigned short index = phase >> SHIFT_AMOUNT;
  if (index >= ARRAY_SIZE(wavetable)) {
    phase = (phase & SHIFT_MASK);
  }

  // Write sample to DAC. Bitshift 4 to left because we have
  // a 8-bit sample but a 12-bit DAC.
  uint16_t sample = pgm_read_byte_near(wavetable+index)<<4;
  dac.write(sample);
}

