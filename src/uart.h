#ifndef UART_H
#define UART_H

#include <avr/io.h>
#include <stdio.h>

/* #define F_CPU 16000000UL */
#define BAUD 9600
#include <util/setbaud.h>


class Uart {

 public:
  Uart() {}
  ~Uart() {}

  static inline void init() {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

    UCSR0A &= ~(1<<U2X0);

    // Set data bit count to 8
    // 011 (UCSZ02,UCSZ01,UCSZ00)
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);

    // Enabled RX and TX
    UCSR0B = (1<<RXEN0) | (1<<TXEN0);
  }

  static inline int uart_putchar(char c, FILE *stream) {
    // Silence error from unused variable.
    (void)stream;
    if (c == '\n') {
      uart_putchar('\r', stream);
    }

    loop_until_bit_is_set(UCSR0A, UDRE0);
    UDR0 = c;

    return c;
  }

  static inline int uart_getchar(FILE *stream) {
    // Silence error from unused variable.
    (void)stream;
    loop_until_bit_is_set(UCSR0A, RXC0);
    return UDR0;
  }

};

// Create streams for output and input.
// NOTICE: fdevopen calls malloc!
FILE *uart_output = fdevopen(Uart::uart_putchar, NULL);
FILE *uart_input = fdevopen(NULL, Uart::uart_getchar);

#endif
