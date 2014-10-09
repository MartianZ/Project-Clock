## By MartianZ
## Last Modified: 2014-07-06 21:51:56

OBJS = main.o delay.o stm32f10x_it.o usb_desc.o usb_endp.o usb_istr.o usb_prop.o usb_pwr.o hw_config.o ds3231.o usart.o nixietube.o flash.o

PROJ_NAME=main


export CC=/Users/MartianZ/sat/bin/arm-none-eabi-gcc
export LD=/Users/MartianZ/sat/bin/arm-none-eabi-gcc
export AR=/Users/MartianZ/sat/bin/arm-none-eabi-ar
export AS=/Users/MartianZ/sat/bin/arm-none-eabi-as
export OBJCOPY=/Users/MartianZ/sat/bin/arm-none-eabi-objcopy

export ASFLAGS=
export LDFLAGS=-Tstm32f100.ld -Llib -fno-common -mcpu=cortex-m3 -mthumb 
export CFLAGS=-O1 -c -fno-common -mcpu=cortex-m3 -mthumb -DSTM32F10X_MD=1 -DUSE_STDPERIPH_DRIVER=1 -DUSE_STM3210B_EVAL=1

CWD = $(shell pwd)
export ROOT=$(CWD)
export LIB_ROOT=$(ROOT)/lib

###################################################

vpath %.c $(ROOT)/src  

CFLAGS += -I$(ROOT)/inc -I$(LIB_ROOT) -I$(LIB_ROOT)/inc
CFLAGS += -I$(LIB_ROOT)/inc/core -I$(LIB_ROOT)/inc/peripherals 

OBJS += $(LIB_ROOT)/startup_stm32f10x_md.s # add startup file to build

###################################################

.PHONY: lib proj

all: 	lib proj flash

lib:
	$(MAKE) -C lib

proj: 	$(PROJ_NAME).elf

$(PROJ_NAME).elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS) -lstm32f10x -lm -lc -lnosys
	$(OBJCOPY) -O ihex $(PROJ_NAME).elf $(PROJ_NAME).hex
	$(OBJCOPY) -O binary $(PROJ_NAME).elf $(PROJ_NAME).bin

clean:
	rm -f *.o
	rm -f lib/*.o
	rm -f lib/libstm32f10x.a
	rm -f $(PROJ_NAME).elf
	rm -f $(PROJ_NAME).hex
	rm -f $(PROJ_NAME).bin
	
flash:
	/Users/MartianZ/Documents/stm32/stlink/st-flash write $(PROJ_NAME).bin 0x08004000
erase:
	/Users/MartianZ/Documents/stm32/stlink/st-flash erase
	
debug1:
	st-util 4242 usb
	
debug2:
	arm-none-eabi-gdb -ex "target remote localhost:4242" main.elf
	
