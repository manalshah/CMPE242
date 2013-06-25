/*
 * User space ADC module for the Tiny6410 Board and ARM11.
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#ifndef ADC_H
#define ADC_H
#include <stdint.h>

float error_derivative( void ) ;
int get_data( void ) ;
int init_adc( int adc_fd, int channel , int bit_resolution ) ;
int validate_data( void ) ;
uint32_t error( void ) ;
uint32_t error_integral( void) ;

#endif // ADC_H
