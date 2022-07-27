src = $(wildcard src/*.c)
obj = $(src:.c=.o)
bin = sgikbd
hex = $(bin).hex
eep = $(bin).eep

mcu_gcc = atmega328pb
mcu_dude = m328pb

CC = avr-gcc
OBJCOPY = avr-objcopy

CFLAGS = -Os -pedantic -Wall -mmcu=$(mcu_gcc) -DF_CPU=3686400
LDFLAGS = -Wl,-Map,$(bin).map -mmcu=$(mcu_gcc)

.PHONY: all
all: $(hex) $(eep)

$(bin): $(obj)
	$(CC) -o $@ $(obj) $(LDFLAGS)

$(hex): $(bin)
	$(OBJCOPY) -j .text -j .data -O ihex -R .eeprom $< $@

$(eep): $(bin)
	$(OBJCOPY) -j .eeprom --change-section-lma .eeprom=0 -O ihex $< $@

.PHONY: fuses
fuses:
	avrdude -c usbtiny -p $(mcu_dude) -U lfuse:w:0xe6:m -U hfuse:w:0xd9:m -U efuse:w:0xf7:m

.PHONY: program
program: $(hex)
	avrdude -c usbtiny -p $(mcu_dude) -e -U flash:w:$(hex)

.PHONY: clean
clean:
	rm -f $(bin) $(obj) $(hex) $(eep) $(bin).map
