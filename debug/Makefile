
obj-m +=  lab3_chrdrv.o lab3_probe_module.o lab4_debugfs.o lab5_proc.o lab8_seqfile.o lab10_procfs.o lab11_seq.o b12_faulty.o lab13_busy.o 

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers

