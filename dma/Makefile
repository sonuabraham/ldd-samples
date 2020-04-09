
 obj-m := lab1_copytofrom.o lab1_putget.o lab2_usermap.o \
 	  lab2_usermap_fast.o lab3_mmap.o  lab5_dma.o \
	  lab5_dma_PCI_API.o lab6_aio.o 

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers
	


