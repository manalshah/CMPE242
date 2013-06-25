/*
 * Header file for fft.
 *
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */
#ifndef FFT_H
#define FFT_H
#include <stdint.h>

typedef struct 
{
	double r; //Real Part
	double i; //Imaginary Part
} Complex ; 

void FFT( Complex * X, uint16_t N ) ;

#endif // FFT_H
