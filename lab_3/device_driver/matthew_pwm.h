/*
 * PWM Device Driver for the Tiny6410 Board and ARM11.
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#ifndef PWM_H
#define PWM_H 

#include <linux/ioctl.h>

#define PWM_DEVICE_NAME "matthew_pwm"

#define PWM_MAGIC 0x44

#define PWM_IOCTL_STOP			_IO(PWM_MAGIC, 0)
#define PWM_IOCTL_SET_FREQ		_IO(PWM_MAGIC, 1)

// reset the device
#define PWM_RESET _IO(PWM_MAGIC, 2)

// error checking
#define PWM_MAXCMD 2
#endif

