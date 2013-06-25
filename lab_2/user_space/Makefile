# ----------------------------------------------------------------------------
# Makefile for building the app
#
# Copyright 2010 FriendlyARM (http://www.arm9.net/)
#
# Modified by
# Matthew Lopez
# CMPE 242
# Spring 2013
# San Jose State University
#

ifndef DESTDIR
DESTDIR			   ?= /tmp/FriendlyARM/mini6410/rootfs
endif

CFLAGS				= -Wall -O2 -lm
CC					= arm-linux-gcc
INSTALL				= install

TARGET				= adc_validation

all: $(TARGET)

$(TARGET): $(TARGET).c fft.o
	$(CC) $(CFLAGS) -I/home/matthew/school/lab_2 $< fft.o -o $@

fft.o: fft.c
	$(CC) -c $(CFLAGS) -I/home/matthew/school/lab_2 $< -o $@


install: $(TARGET)
	$(INSTALL) $^ $(DESTDIR)/usr/bin

clean distclean:
	rm -rf *.o $(TARGET)


# ----------------------------------------------------------------------------

.PHONY: $(PHONY) install clean distclean

# End of file
# vim: syntax=make
