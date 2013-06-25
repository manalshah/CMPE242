/*
 * GPIO Device Driver for the Tiny6410 Board and ARM11.
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#include <linux/fs.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <mach/gpio-bank-e.h>
#include <mach/map.h>
#include <mach/regs-gpio.h>

#include "matthew_gpio.h"

// control values
static const uint32_t GPE1_OUT = 0x0010 ; // forward
static const uint32_t GPE2_OUT = 0x0100 ; // reverse
static const uint32_t GPE3_OUT = 0x1000 ; // debug; complete

// data bits within GPEDAT
static const uint32_t GPE1_DAT = 0X2 ; // forward
static const uint32_t GPE2_DAT = 0X4 ; // reverse
static const uint32_t GPE3_DAT = 0X8 ; // debug; complete

/*
 * Directional Control for the Motor.
 * Debugging indicator is also provided.
 */
static long matthew_gpio_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	uint32_t new_value = 0 ;
	uint32_t current_value = 0 ;

	/*
	 * Basic checking of the command
	 */
	if (_IOC_TYPE(cmd) != GPIO_MAGIC)
		return -ENOTTY ;
	if (_IOC_NR(cmd) > GPIO_MAXCMD)
		return -ENOTTY ;

	current_value = readl(S3C64XX_GPEDAT) ;
	switch(cmd) {
	case GPIO_FORWARD: // Motor is to go forward
		new_value = current_value | GPE1_DAT ;
		writel(new_value, S3C64XX_GPEDAT) ;
		return 0;
	case GPIO_REVERSE: // Motor is to go reverse
		new_value = current_value | GPE2_DAT ;
		writel(new_value, S3C64XX_GPEDAT) ;
		return 0;
	case GPIO_STOP: // Motor is where we want it.
		new_value = current_value & ~(GPE1_DAT | GPE2_DAT) ;
		writel(new_value, S3C64XX_GPEDAT) ;
		return 0 ;
	case GPIO_COMPLETE_ON: // Turn on the debug; completed led
		new_value = current_value | GPE3_DAT ;
		writel(new_value, S3C64XX_GPEDAT) ;
		return 0 ;
	case GPIO_COMPLETE_OFF: // Turn off the debug; completed led
		new_value = current_value & ~GPE3_DAT ;
		writel(new_value, S3C64XX_GPEDAT) ;
	case GPIO_RESET: // turn off everything
		new_value = current_value & ~(GPE1_DAT | GPE2_DAT | GPE3_DAT) ;
		writel(new_value, S3C64XX_GPEDAT) ;
		return 0 ;
	default:
		printk( KERN_ERR "Wrong command. Returning\n" ) ;
		return -ENOTTY;
	}
}

static struct file_operations dev_fops = {
	.owner			= THIS_MODULE,
	.unlocked_ioctl	= matthew_gpio_ioctl,
};

static struct miscdevice misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = GPIO_DEVICE_NAME,
	.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret = 0 ;
	uint32_t new_value = 0 ;
	uint32_t current_value = 0 ;

	/*
	 * Initialize the Port E Control Register
	 * First, reset the control register.
	 * Then turn on/off what we need
	 * PORT BITS  	Value	Purpose
	 * GPE1 [7:4] 	0x1		Output
	 * GPE2 [11:8] 	0x1		Output
	 * GPE3 [15:12] 0x1		Output
	 * All other bits leave as 0x0
	 *
	 */
	writel(0x00, S3C64XX_GPECON) ;
	new_value =  GPE1_OUT | GPE2_OUT | GPE3_OUT ;
	writel(new_value, S3C64XX_GPECON) ;

	/*
	 * Ensure that the data register for Port E is zero
	 */
	writel(0x00, S3C64XX_GPEDAT) ;

	ret = misc_register(&misc);

	current_value = readl(S3C64XX_GPEDAT) ;

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
