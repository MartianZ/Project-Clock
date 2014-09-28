vpath %.c $(LIB_ROOT)/src $(LIB_ROOT)/src/core $(LIB_ROOT)/src/peripherals

CFLAGS+= -I$(LIB_ROOT)/inc -I$(LIB_ROOT)/inc/core -I$(LIB_ROOT)/inc/peripherals

SRCS  = stm32f0xx_misc.c
SRCS += stm32f0xx_adc.c \
	stm32f0xx_dbgmcu.c stm32f0xx_dma.c stm32f0xx_flash.c \
	stm32f0xx_gpio.c stm32f0xx_i2c.c  \
	 stm32f0xx_rcc.c \
	 stm32f0xx_tim.c 
SRCS += system_stm32f0xx.c

OBJS = $(SRCS:.c=.o)

libstm32f0xx.a: $(OBJS)
	$(AR) rcs $@ $(OBJS)

clean:
	rm -f $(OBJS) libstm32f0xx.a
