

obj-m := lab1_timer.o lab2_multitimer.o lab3_periodic_timer.o lab4_periodic_timers.o lab4_periodic_timers_alt.o lab5_hrtimer.o lab6_kprobes.o lab7_hrexamp.o lab8_jiq.o lab9_jit.o 

SRC := $(shell pwd)

all:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC)

modules_install:
	$(MAKE) -C $(KERNEL_SRC) M=$(SRC) modules_install

clean:
	rm -f *.o *~ core .depend .*.cmd *.ko *.mod.c
	rm -f Module.markers Module.symvers modules.order
	rm -rf .tmp_versions Modules.symvers



