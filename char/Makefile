#Makefile for char modules

obj-m += lab1_char_init.o lab2_char_read_write.o lab3_char_ioctl.o lab4_char_seek.o lab5_char_poll.o lab6_char_fasync.o

SRC := $(shell pwd)

all:
	        $(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	        $(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	        rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	        rm -f Module.markers Module.symvers modules.order
	        rm -rf .tmp_versions Modules.symvers


