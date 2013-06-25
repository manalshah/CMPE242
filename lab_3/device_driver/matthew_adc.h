/*
 * ADC Device Driver for the Tiny6410 Board and ARM11.
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#ifndef ADC_DD_H
#define ADC_DD_H 

#include <linux/ioctl.h>

#define ADC_DEVICE_NAME	"matthew_adc"

#define ADC_MAGIC 0x41

// reset the device
#define ADC_RESET _IO(ADC_MAGIC, 0)

// initialization commands
#define ADC_CHANNEL_2 _IO(ADC_MAGIC, 2)
#define ADC_CHANNEL_3 _IO(ADC_MAGIC, 3)
#define ADC_TEN_BIT _IO(ADC_MAGIC, 10)
#define ADC_TWELVE_BIT _IO(ADC_MAGIC, 12)

// error checking
#define ADC_MAXCMD 12

#endif

