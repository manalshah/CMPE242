#
# Simple I/O Device Driver for the Tiny6410 Board
# Matthew Lopez
# CMPE 242
# Spring 2013
# San Jose State University
#
# This is the makefile for the device driver. Copied from
# Jonathan Corbet, Alessandro Rubini, and Greg Kroah-Hartman,
# Linux Device Drivers, Third Edition,
# O'Reilly Media, Sebastopol, CA, 2005
#
# Modified as needed to fit the needs of my project
#

MODULE_NAME = matthew_leds

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
