
obj-m += sync_lab1_mutex1.o sync_lab1_mutex2.o sync_lab1_mutex3.o sync_lab2_sem1.o sync_lab2_sem2.o sync_lab2_sem3.o sync_lab3_spinlock.o sync_lab4_completion.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
