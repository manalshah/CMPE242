/*
 * Simple I/O Device Driver for the Tiny6410 Board
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 *
 * This is the device driver
 */
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <mach/gpio-bank-e.h>
#include <mach/map.h>
#include <mach/regs-gpio.h>

#include "matthew_leds.h"

#define DEVICE_NAME "matthew_leds"

// control values
static const unsigned int GPE1_OFF = 0x00 ;
static const unsigned int GPE3_ON = 0x1000 ;
// data bits within GPEDAT
static const unsigned int GPE1 = 0X2 ;
static const unsigned int GPE3 = 0X8 ;

/*
 * Turn on the led
 * LED is controlled via GPE3
 */
static long matthew_leds_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	unsigned int new_value = 0 ;
	unsigned int current_value = 0 ;
	printk( KERN_ERR "ML: cmd is (%d)\n", cmd ) ;
	current_value = readl(S3C64XX_GPEDAT) ;
	printk( KERN_ERR "ML: read current value (0x%08x)\n", current_value ) ;
	switch(cmd) {
	case TURN_OFF_LED: // Turn off the led
		new_value = current_value & ~GPE3 ;
		printk( KERN_ERR "ML: Turn Off: new value (0x%08x)\n", new_value ) ;
		writel(new_value, S3C64XX_GPEDAT) ;
		printk( KERN_ERR "ML: successfully writel\n" ) ;
		return 0;
	case TURN_ON_LED: // Turn ON the led
		new_value = current_value | GPE3 ;
		printk( KERN_ERR "ML: Turn On: new value (0x%08x)\n", new_value ) ;
		writel(new_value, S3C64XX_GPEDAT) ;
		printk( KERN_ERR "ML: successfully writel\n" ) ;
		return 0;
	case READ_SWITCH: // Read the status of the input switch
		return (current_value & GPE1) ;
	default:
		printk( KERN_ERR "ML: Wrong command. Returning\n" ) ;
		return -ENOTTY;
	}
}

static struct file_operations dev_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= matthew_leds_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret = 0 ;
	unsigned int new_value = 0 ;
	unsigned int current_value = 0 ;

	/*
	 * Initialize the Port E Control Register
	 * First, reset the control register.
	 * Then turn on/off what we need
	 * PORT BITS  	Value	Purpose
	 * GPE1 [7:4] 	0x0		Input
	 * GPE3 [15:12] 0x0		Output
	 * All other bits leave as 0x0
	 *
	 */
	writel(0x00, S3C64XX_GPECON) ;
	new_value =  GPE1_OFF | GPE3_ON ;
	writel(new_value, S3C64XX_GPECON) ;

	/*
	 * Ensure that the data register for Port E is zero
	 */
	writel(0x00, S3C64XX_GPEDAT) ;

	ret = misc_register(&misc);

	printk (DEVICE_NAME"\tinitialized\n") ;
	current_value = readl(S3C64XX_GPEDAT) ;
	printk( KERN_ERR "ML: INIT: GPEDAT current value (0x%08x)\n", current_value ) ;
	current_value = readl(S3C64XX_GPECON) ;
	printk( KERN_ERR "ML: INIT: GPECON current value (0x%08x)\n", current_value ) ;

	return ret;
}

static void __exit dev_exit(void)
{
	misc_deregister(&misc) ;
}

module_init(dev_init) ;
module_exit(dev_exit) ;
MODULE_LICENSE("GPL") ;
MODULE_AUTHOR("Matthew Lopez") ;
