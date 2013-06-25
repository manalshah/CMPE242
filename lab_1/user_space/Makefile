# ----------------------------------------------------------------------------
# Makefile for building tapp
#
# Copyright 2010 FriendlyARM (http://www.arm9.net/)
#
# Modified by Matthew Lopez. Added addtional include path
#

ifndef DESTDIR
DESTDIR			   ?= /tmp/FriendlyARM/mini6410/rootfs
endif

CFLAGS				= -Wall -O2
CC					= arm-linux-gcc
INSTALL				= install

TARGET				= led


all: $(TARGET)

led: led.c
	$(CC) $(CFLAGS) -I/home/matthew/school/lab_1 $< -o $@


install: $(TARGET)
	$(INSTALL) $^ $(DESTDIR)/usr/bin

clean distclean:
	rm -rf *.o $(TARGET)


# ----------------------------------------------------------------------------

.PHONY: $(PHONY) install clean distclean

# End of file
# vim: syntax=make

