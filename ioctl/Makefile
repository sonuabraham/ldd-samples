
obj-m := lab1_ioctl_data.o lab2_ioctl_vardata.o lab3_ioctl_signal.o lab4_complete.o lab4_excl.o lab4_semaphore.o lab4_semaphore_rw.o lab4_wait_event.o lab5_killable.o lab6_poll.o lab7_sleepy.o

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers


