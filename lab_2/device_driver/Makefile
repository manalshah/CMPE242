MODULE_NAME = matthew_adc

ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULE_NAME).o

else
	KERNELDIR = /home/matthew/school/OpenSource_Linux_Development_for_ARM11_v1.0/Linux/linux-2.6.38
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) -Wall modules

endif

clean:
	rm -f $(MODULE_NAME).ko $(MODULE_NAME).mod.c $(MODULE_NAME).mod.o $(MODULE_NAME).o modules.order Module.symvers
