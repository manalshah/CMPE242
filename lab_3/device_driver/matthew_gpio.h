/*
 * GPIO Device Driver for the Tiny6410 Board and ARM11.
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#ifndef GPIO_H
#define GPIO_H 

#include <linux/ioctl.h>

#define GPIO_DEVICE_NAME "matthew_gpio"

#define GPIO_MAGIC 0x43

// what direction to move the motor
#define GPIO_FORWARD _IO(GPIO_MAGIC, 0)
#define GPIO_REVERSE _IO(GPIO_MAGIC, 1)
#define GPIO_STOP _IO(GPIO_MAGIC, 2)

// used for debuging
#define GPIO_COMPLETE_ON _IO(GPIO_MAGIC, 3)
#define GPIO_COMPLETE_OFF _IO(GPIO_MAGIC, 4)

// reset the device
#define GPIO_RESET _IO(GPIO_MAGIC, 5)

// error checking
#define GPIO_MAXCMD 5
#endif
