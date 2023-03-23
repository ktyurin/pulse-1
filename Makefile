#

CC = xc8-cc

MP_PROCESSOR_OPTION = 16F15223

BUILD_DIR = build


CFLAGS = -fno-short-double \
         -fno-short-float \
         -O1 \
         -maddrqual=ignore \
         -mwarn=0 \
         -Wa,-a \
         -msummary=-psect,-class,+mem,-hex,+file \
         -ginhx32 \
         -Wl,--data-init \
         -mno-osccal \
         -mno-resetbits \
         -mno-save-resetbits \
         -mno-download \
         -mno-stackcall \
         -mdefault-config-bits \
         -std=c99 \
         -gdwarf-3 \
         -mstack=compiled:auto:auto


LDFLAGS = -Wl,-Map=$(BUILD_DIR)/fw.map  \
          -Wl,--defsym=__MPLAB_BUILD=1 \
          -fno-short-double \
          -fno-short-float \
          -O1 \
          -maddrqual=ignore \
          -mwarn=0 \
          -Wa,-a \
          -msummary=-psect,-class,+mem,-hex,+file \
          -ginhx32 -Wl,--data-init \
          -mkeep-startup \
          -mno-osccal \
          -mno-resetbits \
          -mno-save-resetbits \
          -mno-download \
          -mno-stackcall \
          -mdefault-config-bits \
          -std=c99 \
          -gdwarf-3 \
          -mstack=compiled:auto:auto \
	  -Wl,--memorysummary,$(BUILD_DIR)/memoryfile.xml


SRC_FILES = main.c sleep.c system.c device_config.c \
    peripherals/adc/adc.c \
    peripherals/gpio/gpio.c \
    peripherals/interrupts/interrupts.c \
    peripherals/oscillator/oscillator.c \
    peripherals/watchdog/watchdog.c


OBJECT_FILES := $(addprefix $(BUILD_DIR)/,$(patsubst %.c,%.p1,$(SRC_FILES)))


$(BUILD_DIR)/%.p1 : %.c
	mkdir -p $(@D)
	$(CC) -mcpu=$(MP_PROCESSOR_OPTION) -c $(CFLAGS) -o $@ $<


$(BUILD_DIR)/fw.hex: $(OBJECT_FILES)
	$(CC) -mcpu=$(MP_PROCESSOR_OPTION) -o $(BUILD_DIR)/fw.hex  $(OBJECT_FILES)     
