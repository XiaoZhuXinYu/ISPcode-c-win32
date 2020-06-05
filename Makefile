#
#Makefile for main.c
#

CC = /home/sc/tina30_0605/out/banjo-R11_pref1/staging_dir/toolchain/bin/arm-openwrt-linux-muslgnueabi-gcc-6.4.1
#CC := /home/sc/mr100_scanner_sdk_v200/out/sun8iw8p1/linux/common/buildroot/external-toolchain/bin/arm-buildroot-linux-musleabihf-gcc-4.9.4
CFLAGS = -Wall -O3
WL = -Wl,-rpath=/mnt/SDCARD/


TAG_NAME = chenjun_out

INCLUDES = ./include/
DIR_BIN = ./bin/
C_SOURCES = bmpimage.c cj_2dnr.c cj_debug.c cj_base_alg.c main.c
#C_COBJS = is_ad_guard_client.o
C_COBJS = $(patsubst %.c, %.o, $(C_SOURCES))

LIBS = -lpthread

#-o为指定输出文件名, -c为只编译不链接，编写makefile,gcc前不能使用空格
#	$(CC) -shared -o $(TAG_PATH)$(TAG_NAME)  $^
#它告诉make 后面的命令 使用从.c文件生成.o，相当于%.o : %.c$(LIBS)
#makefile文件中，make 不跟参数，只会执行makefile中第一条（带冒号的语句）
.c.o:
	$(CC) $(CFLAGS) -c -o $*.o -I$(INCLUDES) $*.c

all:$(C_COBJS)
	$(CC) $(WL) -o $(DIR_BIN)$(TAG_NAME)  $^  $(LIBS)
	mv $(C_COBJS) ./bin/obj
clean:
	rm -f $(DIR_BIN)$(TAG_NAME)
	# rm -f $(C_COBJS)

redo: clean all

