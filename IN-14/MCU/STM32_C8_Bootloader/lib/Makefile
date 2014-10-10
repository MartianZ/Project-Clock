vpath %.c $(LIB_ROOT)/src $(LIB_ROOT)/src/core $(LIB_ROOT)/src/peripherals

CFLAGS+= -I$(LIB_ROOT)/inc -I$(LIB_ROOT)/inc/core -I$(LIB_ROOT)/inc/peripherals

SRCS  = misc.c
SRCS += stm32f10x_gpio.c \
	stm32f10x_tim.c \
	stm32f10x_exti.c stm32f10x_pwr.c stm32f10x_usart.c \
	stm32f10x_flash.c stm32f10x_rcc.c  \
	 usb_core.c usb_init.c usb_int.c usb_mem.c usb_regs.c usb_sil.c
SRCS += core_cm3.c system_stm32f10x.c

OBJS = $(SRCS:.c=.o)

libstm32f10x.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -f $(OBJS) libstm32f10x.a
