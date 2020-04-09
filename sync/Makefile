
obj-m += lab1_mutex1.o lab1_mutex2.o lab1_mutex3.o lab2_sem1.o lab2_sem2.o lab2_sem3.o lab3_unlock.o 

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
