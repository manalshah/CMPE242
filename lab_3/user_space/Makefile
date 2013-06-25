# ----------------------------------------------------------------------------
# Makefile for building the app
#
# Modified by
# Matthew Lopez
# CMPE 242
# Spring 2013
# San Jose State University
#
# Original 2010 FriendlyARM (http://www.arm9.net/)
#

INCLUDE				= /home/matthew/school/lab_3
#CFLAGS				= -Wall -O2 -D DEBUG -D TESTING
#CFLAGS				= -Wall -O2  -D DEBUG
CFLAGS				= -Wall -O2 
LDFLAGS				= -lm 
CC					= arm-linux-gcc
TARGET				= run_motor
OBJECTS				= $(SOURCES:.c=.o)
SOURCES				= fft.c adc.c run_motor.c
FTP_DIR				= /srv/ftp

all: $(SOURCES) $(TARGET)
	
$(TARGET): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) -c $(CFLAGS) -I$(INCLUDE) $< -o $@

pwm_test:	pwm_test.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(INCLUDE) pwm_test.c -o pwm_test

install:	$(TARGET)
	sudo cp $(TARGET) $(FTP_DIR)


clean distclean:
	rm -rf *.o $(TARGET)


# ----------------------------------------------------------------------------

.PHONY: $(PHONY) clean distclean

# End of file
# vim: syntax=make
