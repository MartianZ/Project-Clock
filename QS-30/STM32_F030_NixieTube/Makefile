## By MartianZ

OBJS = main.o stm32f0xx_it.o pid.o nixietube.o delay.o ds1302.o

PROJ_NAME=main


export CC=/Users/MartianZ/Documents/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-gcc
export LD=/Users/MartianZ/Documents/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-gcc
export AR=/Users/MartianZ/Documents/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-ar
export AS=/Users/MartianZ/Documents/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-as
export OBJCOPY=/Users/MartianZ/Documents/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-objcopy
export STRIP=/Users/MartianZ/Documents/gcc-arm-none-eabi-4_8-2014q2/bin/arm-none-eabi-strip

export ASFLAGS=
export LDFLAGS=-Llib -Tstm32f0_linker.ld -nostartfiles -mthumb -mcpu=cortex-m0 -Os
export CFLAGS=-mcpu=cortex-m0 -mthumb -fverbose-asm -Os -DUSE_STDPERIPH_DRIVER -DSTM32F0XX -DPLL_SOURCE_HSI

CWD = $(shell pwd)
export ROOT=$(CWD)
export LIB_ROOT=$(ROOT)/lib


vpath %.c $(ROOT)/src  

CFLAGS += -I$(ROOT)/inc -I$(LIB_ROOT) -I$(LIB_ROOT)/inc
CFLAGS += -I$(LIB_ROOT)/inc/core -I$(LIB_ROOT)/inc/peripherals 

OBJS += $(LIB_ROOT)/startup_stm32f0xx.s

.PHONY: lib proj

all: 	lib proj flash

lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) -lstm32f0xx
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin


clean:
	rm -f *.o
	rm -f lib/*.o
	rm -f lib/libstm32f0xx.a
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	
flash:
	/Users/MartianZ/Documents/stm32/stlink/st-flash write $(PROJ_NAME).bin 0x08000000
erase:
	/Users/MartianZ/Documents/stm32/stlink/st-flash erase
	
debug1:
	st-util 4242 usb
	
debug2:
	arm-none-eabi-gdb -ex "target remote localhost:4242" main.elf
	
