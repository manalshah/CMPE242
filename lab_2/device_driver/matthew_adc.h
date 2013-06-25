#ifndef MATTHEW_ADC_H
#define MATTHEW_ADC_H 

#include <linux/ioctl.h>

#define DEVICE_NAME	"matthew_adc"

#define MATTHEW_ADC_MAGIC 24

#define CHANNEL_2 _IO(MATTHEW_ADC_MAGIC, 2)
#define CHANNEL_3 _IO(MATTHEW_ADC_MAGIC, 3)
#define TEN_BIT _IO(MATTHEW_ADC_MAGIC, 10)
#define TWELVE_BIT _IO(MATTHEW_ADC_MAGIC, 12)

#endif

