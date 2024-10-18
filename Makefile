#==========================================================
#	File:	Makefile for Cortex-M3
#	Date:	2011-04-17
#==========================================================

# ����������� �� ������� ��������� ����
OPTIMIZATION = s 

# �������� ���� ����� � ������, ��� ��������� ����� ������
# �� ��������� ���� ������ ���������
SRC_C = startup.c
SRC_C += main.c
SRC_C += system_stm32f10x.c
SRC_C += stm32f10x_it.c

SRC_C += misc.c
SRC_C += stm32f10x_adc.c
# SRC_C += stm32f10x_bkp.c
# SRC_C += stm32f10x_can.c
# SRC_C += stm32f10x_cec.c
# SRC_C += stm32f10x_crc.c
# SRC_C += stm32f10x_dac.c
# SRC_C += stm32f10x_dbgmcu.c
SRC_C += stm32f10x_dma.c
# SRC_C += stm32f10x_exti.c
SRC_C += stm32f10x_flash.c
# SRC_C += stm32f10x_fsmc.c
SRC_C += stm32f10x_gpio.c
# SRC_C += stm32f10x_i2c.c
SRC_C += stm32f10x_iwdg.c
# SRC_C += stm32f10x_pwr.c
SRC_C += stm32f10x_rcc.c
# SRC_C += stm32f10x_rtc.c
SRC_C += stm32f10x_sdio.c
SRC_C += stm32f10x_spi.c
SRC_C += stm32f10x_tim.c
# SRC_C += stm32f10x_usart.c
# SRC_C += stm32f10x_wwdg.c

#----------------------------------------------------------

# ��������� ���� �� GCC, � ��� �� ���� � ����������� ��� �������� 
# ����� �������� ����� � ������������ �����

#CROSS_PATH = C:/Tools/CodeSourcery/2010-09-51/
CROSS_PATH = "C:\Tools\GNU-Tools-ARM-Embedded\6-2017-q1-update/"

CROSS = $(CROSS_PATH)/bin/arm-none-eabi-

INCLUDES += -I$(CROSS_PATH)/arm-none-eabi/include
INCLUDES += -I$(CROSS_PATH)/arm-none-eabi/include/lib
INCLUDES += -ILibraries/IOlibrary/Ethernet
INCLUDES += -ILibraries/IOlibrary/Internet
INCLUDES += -Ixprintf

INCLUDES += -ISystem
INCLUDES += -IStdPeriph_Driver/inc
INCLUDES += -IStdPeriph_Driver/src

VPATH += System
VPATH += StdPeriph_Driver/inc
VPATH += StdPeriph_Driver/src

#-------------------- ����� ���������� ---------------------

FLAGS_C  = $(INCLUDES) -I.
FLAGS_C += -O$(OPTIMIZATION)
FLAGS_C += -Wall
FLAGS_C += -c -w -n 
FLAGS_C += -fmessage-length=0
FLAGS_C += -fno-builtin
FLAGS_C += -ffunction-sections
FLAGS_C += -fdata-sections
FLAGS_C += -msoft-float
FLAGS_C += -mapcs-frame
FLAGS_C += -D__thumb2__=1
FLAGS_C += -mno-sched-prolog
FLAGS_C += -fno-hosted
FLAGS_C += -mtune=cortex-m3
FLAGS_C += -mcpu=cortex-m3
FLAGS_C += -mthumb
FLAGS_C += -mfix-cortex-m3-ldrd
FLAGS_C += -fno-strict-aliasing

FLAGS_LD = -Xlinker -Map=target/target.map
FLAGS_LD += -Wl,--gc-sections
FLAGS_LD += -mcpu=cortex-m3
FLAGS_LD += -mthumb
FLAGS_LD += -static   
#FLAGS_LD += -nostdlib


#----------------------------------------------------------

all: clean target.elf

%.elf: $(SRC_C:%.c=target/%.o)
	@echo Linking: $@
	@$(CROSS)gcc $(FLAGS_LD) -T'System/stm32f100c4t6.lsf' -o 'target/$@' $^ $(LD_LIB)
	@echo '-----------------------------------------------------------'
	@$(CROSS)size 'target/target.elf'
	@$(CROSS)objcopy -O binary 'target/target.elf' 'target/target.bin'
	@$(CROSS)objcopy -O ihex 'target/target.elf' 'target/target.hex'
	@$(CROSS)objdump -h -S -z 'target/target.elf' > 'target/target.lss'
	@$(CROSS)nm -n 'target/target.elf' > 'target/target.sym'
	@rm -f target/*.o

$(SRC_C:%.c=target/%.o): target/%.o: %.c
	@echo Compiling: $<
	@$(CROSS)gcc $(FLAGS_C) -c $< -o $@

clean:
	@echo '-----------------------------------------------------------'
	@rm -f target/*.*	

.PHONY : all clean