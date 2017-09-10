obj-m:=hello_world.o

KERNELDIR :=/lib/modules/$(shell uname -r)/build
PWD	:=$(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)

clean:
	rm -rf *.o *.ko modules.order Module.symvers hello_world.mod.c 


