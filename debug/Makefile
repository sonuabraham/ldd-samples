# Comment/uncomment the following line to disable/enable debugging
DEBUG = y
# Add your debugging flag (or not) to CFLAGS
ifeq ($(DEBUG),y)
 CFLAGS_debug_lab1_print.o = -O -g -DLDD_DEBUG
else
 CFLAGS_debug_lab1_print.o = -O2
endif
obj-m +=  debug_lab1_print.o debug_lab2_procfs.o debug_lab3_seqfile.o
#obj-m +=  debug_lab1_print.o debug_lab2_procfs.o debug_lab3_seqfile.o debug_lab4_debugfs.o debug_lab5_kprobes.o debug_lab6_jprobes.o 
			
SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

