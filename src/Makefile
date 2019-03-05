baud=115200
avr=atmega328p
freq=16000000
dev=/dev/cu.usbmodem1421
programmer=arduino

cflags=-DF_CPU=$(freq) -mmcu=$(avr) -Wall -Werror -Wextra -Os -std=c++11
objects=$(patsubst %.cc,%.o,$(wildcard *.cc))

.PHONY: flash clean

all: main.hex

%.o: %.cc
	avr-gcc $(cflags) -c $< -o $@

main.elf: $(objects)
	avr-gcc $(cflags) -o $@ $^

main.hex: main.elf
	avr-objcopy -j .text -j .data -O ihex $^ $@

flash: main.hex
	avrdude -p$(avr) -c$(programmer) -P$(dev) -b$(baud) -v -U flash:w:$<

clean:
	rm -f main.hex main.elf $(objects)
