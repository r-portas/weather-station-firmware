PRG = main
# MCU_TARGET = atmega328
MCU_TARGET = atmega324p

PROGRAMMER = avrisp2
CPU_SPEED = 8000000UL

BAUD_RATE = 9600

SRC = build/serial.o build/digital.o build/main.o build/adc.o build/interrupts.o build/eeprom.o

#
# DON'T CHANGE THINGS BELOW THIS LINE OR BAD THINGS HAPPEN ;)
#

CC = avr-gcc
OBJCOPY	= avr-objcopy

CFLAGS=-Os -DF_CPU=$(CPU_SPEED) -mmcu=$(MCU_TARGET) -std=c99

all: build/$(PRG).hex

build/$(PRG).hex: build/$(PRG).elf
	$(OBJCOPY) -O ihex -R .eeprom build/$(PRG).elf build/$(PRG).hex

build/$(PRG).elf: $(SRC)
	$(CC) -mmcu=$(MCU_TARGET) -o build/$(PRG).elf $(SRC)

build/%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

install: build/$(PRG).hex
	avrdude -c $(PROGRAMMER) -p $(MCU_TARGET) -U flash:w:build/$(PRG).hex

clean:
	rm -v build/*.hex build/*.elf build/*.o

burn_328_fuses:
	avrdude -b $(BAUD_RATE) -c $(PROGRAMMER)  -p m328 -U lfuse:w:0xe2:m

burn_324_fuses:
	avrdude -b $(BAUD_RATE) -c $(PROGRAMMER) -p m324p -U lfuse:w:0xc2:m
