#ifndef SPI_H
#define SPI_H

#include <avr/io.h>
#include <avr/interrupt.h>

class Spi {
    public:
        Spi(){}
        ~Spi(){}

    static inline void init() {
        DDRB |= (1<<PB5) | (1<<PB3) | (1<<PB2); // SCK and MOSI
        SPCR = (1<<MSTR) | (1<<SPR0) | (1<<SPE); //Master mode
    }

    static inline void write(uint8_t byte) {
        SPDR = byte;
        // Wait for completion.
        while(!(SPSR & (1<<SPIF)));
    }
};

#endif
