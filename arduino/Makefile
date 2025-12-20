# Makefile for Arduino Uno Bare Metal C Project
# ATmega328P Compilation

# Project name
PROJECT = smart_fan

# Microcontroller
MCU = atmega328p
F_CPU = 16000000UL

# Compiler and tools
CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
AVRDUDE = avrdude

# Compiler flags
CFLAGS = -Wall -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU) -std=c99
LDFLAGS = -mmcu=$(MCU)

# Source files
SOURCES = main.c uart.c ultrasonic.c
OBJECTS = $(SOURCES:.c=.o)

# AVRDUDE settings (adjust for your programmer)
# For USBasp: -c usbasp
# For Arduino as ISP: -c arduino -P COM3 (Windows) or /dev/ttyUSB0 (Linux)
# For direct USB: -c arduino -P COM3 -b 115200
PROGRAMMER = arduino
PORT = COM3
BAUDRATE = 115200

# Default target
all: $(PROJECT).hex

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create ELF
$(PROJECT).elf: $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)
	$(SIZE) $@

# Convert ELF to Intel HEX format
$(PROJECT).hex: $(PROJECT).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@

# Disassemble (for debugging)
disasm: $(PROJECT).elf
	$(OBJDUMP) -d $<

# Upload to Arduino
upload: $(PROJECT).hex
	$(AVRDUDE) -c $(PROGRAMMER) -p $(MCU) -P $(PORT) -b $(BAUDRATE) -U flash:w:$(PROJECT).hex:i

# Clean build files
clean:
	rm -f *.o *.elf *.hex

# Help
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  upload  - Build and upload to Arduino"
	@echo "  clean   - Remove build files"
	@echo "  disasm  - Generate disassembly"
	@echo "  help    - Show this help"

.PHONY: all upload clean disasm help

