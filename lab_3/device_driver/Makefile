#
# Makefile for building device drivers.
# This makefile requires a bash script to set the MODULE_NAME because there
# multiple device drivers to build and this makefile works on one file at a time.
#
# Modified by
# Matthew Lopez
# CMPE 242
# Spring 2013
#
# San Jose State University
# Inspired by
# Jonathan Corbet, Alessandro Rubini, and Greg Kroah-Hartman,
# Linux Device Drivers, Third Edition,
# O'Reilly Media,
# Sebastopol, CA,
# 2005
#
MODULE_NAME = default
FTP_DIR				= /srv/ftp

ifneq ($(KERNELRELEASE),)
	obj-m := $(MODULE_NAME).o

else
	KERNELDIR = /home/matthew/school/OpenSource_Linux_Development_for_ARM11_v1.0/Linux/linux-2.6.38
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) -Wall modules

endif

install:
	sudo cp *.ko $(FTP_DIR)

clean:
	rm -f *.ko *.mod.c *.mod.o *.o modules.order Module.symvers

