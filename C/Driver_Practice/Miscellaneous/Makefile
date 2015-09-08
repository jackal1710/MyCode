ifneq ($(KERNELRELEASE),)
	obj-m += hello_world.o
else
	KERNELDIR := /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)

modules:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

clean:
	rm -rf *.o *.ko temp*

.PHONY: modules clean

endif
