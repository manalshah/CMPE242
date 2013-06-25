/*
 * Simple I/O Device Driver for the Tiny6410 Board
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 *
 * This is the device driver
 */

#ifndef MATTHEW_LEDS_H
#define MATTHEW_LEDS_H 

#include <linux/ioctl.h>

#define MATTHEW_LED_MAGIC 42

#define TURN_OFF_LED _IO(MATTHEW_LED_MAGIC, 0)
#define TURN_ON_LED _IO(MATTHEW_LED_MAGIC, 1)
#define READ_SWITCH _IO(MATTHEW_LED_MAGIC, 2)
#endif
