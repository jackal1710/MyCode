ifneq ($(KERNELRELEASE),)
	obj-m += lab11_5.o 
else
	KERNELDIR := /lib/modules/$(shell uname -r)/build/
	PWD := $(shell pwd)

modules:
	$(MAKE) C=2 -C $(KERNELDIR) M=$(PWD) modules

modules_install:
	$(MAKE) C=2 -C $(KERNELDIR) M=$(PWD) modules_install

clean:
	rm -rf *.o temp* *.mod* *.symvers *.order

.PHONY: modules clean

endif
