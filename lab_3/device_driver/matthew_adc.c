/*
 * ADC Device Driver for the Tiny6410 Board and ARM11.
 * Modified by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 *
 * Original written by
 * Dr. Harry Li,
 * Dept. of Computer Engineering
 * San Jose State University
 *
 * ADC Channel 3 is used.
 */

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/serio.h>
#include <linux/delay.h>
#include <linux/clk.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/cdev.h>
#include <linux/miscdevice.h>

#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>

#include <mach/map.h>
#include <mach/regs-clock.h>
#include <mach/regs-gpio.h>
#include <plat/regs-timer.h>
#include <plat/regs-adc.h>

#include "matthew_adc.h"

#undef DEBUG
//#define DEBUG
#ifdef DEBUG
#define DPRINTK(x...) {printk(__FUNCTION__"(%d): ",__LINE__);printk(##x);}
#else
#define DPRINTK(x...) (void)(0)
#endif


static void __iomem *base_addr;

typedef struct {
	wait_queue_head_t wait;
	uint16_t channel;
	uint16_t prescale;
	uint32_t bits ;
} ADC_DEV;

#ifdef CONFIG_TOUCHSCREEN_MINI6410
extern int mini6410_adc_acquire_io(void);
extern void mini6410_adc_release_io(void);
#else
static inline int mini6410_adc_acquire_io(void) {
	return 0;
}
static inline void mini6410_adc_release_io(void) {
	/* Nothing */
}
#endif

static uint8_t __ADC_locked = 0;

static ADC_DEV adcdev;
static volatile int ev_adc = 0;
static int adc_data;

static struct clk	*adc_clock;

#define __ADCREG(name)	(*(volatile unsigned long *)(base_addr + name))
#define ADCCON			__ADCREG(S3C_ADCCON)	// ADC control
#define ADCTSC			__ADCREG(S3C_ADCTSC)	// ADC touch screen control
#define ADCDLY			__ADCREG(S3C_ADCDLY)	// ADC start or Interval Delay
#define ADCDAT0			__ADCREG(S3C_ADCDAT0)	// ADC conversion data 0
#define ADCDAT1			__ADCREG(S3C_ADCDAT1)	// ADC conversion data 1
#define ADCUPDN			__ADCREG(S3C_ADCUPDN)	// Stylus Up/Down interrupt status

#define PRESCALE_DIS		(0 << 14)
#define PRESCALE_EN			(1 << 14)
#define PRSCVL(x)			((x) << 6)
#define ADC_INPUT(x)		((x) << 3)
#define ADC_START			(1 << 0)
#define ADC_ENDCVT			(1 << 15)
#define ADC_10BIT			(0 << 16)
#define ADC_12BIT			(1 << 16)
#define CHANNEL_2 2
#define CHANNEL_3 3

#define START_ADC_AIN(ch, prescale, bits) \
	do { \
		ADCCON = PRESCALE_EN | PRSCVL(prescale) | ADC_INPUT((ch)) | bits ; \
		ADCCON |= ADC_START; \
	} while (0)


static irqreturn_t adcdone_int_handler(int irq, void *dev_id)
{
	if (__ADC_locked) {
		adc_data = ADCDAT0 & 0x3ff;

		ev_adc = 1;
		wake_up_interruptible(&adcdev.wait);

		/* clear interrupt */
		__raw_writel(0x0, base_addr + S3C_ADCCLRINT);
	}

	return IRQ_HANDLED;
}

static ssize_t s3c2410_adc_read(struct file *filp, char *buffer, size_t count, loff_t *ppos)
{
	char str[20];
	int value = 0;
	size_t len = 0;
	unsigned long len_copy = 0 ;

	memset( str, 0x0, 20 ) ;

	/* Begin Commented Out
	printk( KERN_ERR "ML: read: channel (%d), bits (%d)\n", adcdev.channel,adcdev.bits);
	End Commented Out */

	if (mini6410_adc_acquire_io() == 0) {
		__ADC_locked = 1;

		START_ADC_AIN(adcdev.channel, adcdev.prescale, adcdev.bits);
		/* Begin Commented Out
		printk( KERN_ERR "ML: read: ADCON current value (0x%x)\n", ADCCON & 0xFFFFF ) ;
		End Commented Out */

		wait_event_interruptible(adcdev.wait, ev_adc);
		ev_adc = 0;

		DPRINTK("AIN[%d] = 0x%04x, %d\n", adcdev.channel, adc_data, ADCCON & 0x80 ? 1:0);

		value = adc_data;

		__ADC_locked = 0;
		mini6410_adc_release_io();
	} else {
		value = -1;
	}

	len = sprintf(str, "%d\n", value);
	if (count >= len) {
		len_copy = copy_to_user(buffer, str, len);
		return len_copy ? len_copy : len;
	} else {
		return -EINVAL;
	}
}

static int s3c2410_adc_open(struct inode *inode, struct file *filp)
{
	init_waitqueue_head(&(adcdev.wait));

	adcdev.prescale=0xff;

	DPRINTK("adc opened (%d)\n", adcdev.channel);
	return 0;
}

/*
 * Allow the user to set the channel and bits
 */
static long matthew_adc_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	printk( KERN_ERR "matthew_adc: ioctl: cmd (%d)\n", cmd ) ;

	/*
	 * Basic checking of the command
	 */
	if (_IOC_TYPE(cmd) != ADC_MAGIC)
		return -ENOTTY ;
	if (_IOC_NR(cmd) > ADC_MAXCMD)
		return -ENOTTY ;

	switch(cmd) {
		case ADC_CHANNEL_2:
			adcdev.channel = CHANNEL_2 ;
			break ;
		case ADC_CHANNEL_3:
			adcdev.channel = CHANNEL_3 ;
			break ;
		case ADC_TEN_BIT:
			adcdev.bits = ADC_10BIT ;
			break ;
		case ADC_TWELVE_BIT:
			adcdev.bits = ADC_12BIT ;
			break ;
		default:
			printk( KERN_ERR "matthew_adc: Wrong command. Returning\n" ) ;
			return -ENOTTY;
	}

	/* Begin Commented Out
	printk( KERN_ERR "ML: ioctl: channel (%d), bits (%d)\n", adcdev.channel,adcdev.bits);
	End Commented Out */
	return 0 ;
}

static int s3c2410_adc_release(struct inode *inode, struct file *filp)
{
	DPRINTK("adc closed\n");
	return 0;
}


static struct file_operations dev_fops = {
	owner:	THIS_MODULE,
	open:	s3c2410_adc_open,
	read:	s3c2410_adc_read,	
	release:	s3c2410_adc_release,
	.unlocked_ioctl	= matthew_adc_ioctl,
};

static struct miscdevice misc = {
	.minor	= MISC_DYNAMIC_MINOR,
	.name	= ADC_DEVICE_NAME,
	.fops	= &dev_fops,
};

static int __init dev_init(void)
{
	int ret;

	base_addr = ioremap(SAMSUNG_PA_ADC, 0x20);
	if (base_addr == NULL) {
		printk(KERN_ERR "Failed to remap register block\n");
		return -ENOMEM;
	}

	adc_clock = clk_get(NULL, "adc");
	if (!adc_clock) {
		printk(KERN_ERR "failed to get adc clock source\n");
		return -ENOENT;
	}
	clk_enable(adc_clock);
	
	/* normal ADC */
	ADCTSC = 0;

	ret = request_irq(IRQ_ADC, adcdone_int_handler, IRQF_SHARED, ADC_DEVICE_NAME, &adcdev);
	if (ret) {
		iounmap(base_addr);
		return ret;
	}

	ret = misc_register(&misc);

	/*
	 * Set Defaults
	 */
	adcdev.channel=CHANNEL_3;
	adcdev.bits=ADC_10BIT;

	printk (ADC_DEVICE_NAME"\tinitialized\n");
	return ret;
}

static void __exit dev_exit(void)
{
	free_irq(IRQ_ADC, &adcdev);
	iounmap(base_addr);

	if (adc_clock) {
		clk_disable(adc_clock);
		clk_put(adc_clock);
		adc_clock = NULL;
	}

	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matthew Lopez");

