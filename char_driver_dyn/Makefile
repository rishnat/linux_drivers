obj-m:=char_driver_dyn.o

KERNELDIR :=/lib/modules/$(shell uname -r)/build
PWD	:=$(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	rm -rf *.o *.ko modules.order Module.symvers *.mod.c 


