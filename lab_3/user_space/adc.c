/*
 * User space ADC module for the Tiny6410 Board and ARM11.
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "fft.h"
#include <sys/ioctl.h>
#include <device_driver/matthew_adc.h>
#include "adc.h"


#ifdef DEBUG
	static const uint32_t sleep_time = 500 * 1000 ; // half a second
	#define NUM_DATA_PTS 16
#else
	static const uint32_t sleep_time = 100 * 1000 ; // tenth of a second
	#define NUM_DATA_PTS 1024
#endif // DEBUG

static uint32_t raw_data[NUM_DATA_PTS] ;
static uint32_t next_data[NUM_DATA_PTS] ;

static int _adc_fd = 0 ;

int get_data( void )
{
	char buffer[30];
	int len = 0 ;
	int value = -1 ;

	memset( buffer, 0x0, sizeof(char)*30) ;
	len = read(_adc_fd, buffer, sizeof buffer -1);
	if (len > 0) {
		buffer[len] = '\0';
		sscanf(buffer, "%d", &value);
	}
	else {
		perror("read ADC device:");
		return -1;
	}

	return value ;
}

int validate_data( void )
{
	int i = 0 ;

	// create the Complex array
	Complex data[NUM_DATA_PTS+1] ;
	Complex orig_data[NUM_DATA_PTS+1] ;
	double power[NUM_DATA_PTS+1] ;
	double total_power = 0.0 ;

	// we don't use the zero'th element, set it to zero
	data[0].r = 0.0 ;
	data[0].i = 0.0 ;
	orig_data[0].r = 0.0 ;
	orig_data[0].i = 0.0 ;
	
#ifdef DEBUG
	printf( "DEBUG: validate_data: begin \n" ) ;
#endif // DEBUG

	/*
	 * Collect the data
	 */
	for( i = 1 ; i <= NUM_DATA_PTS; i++ )  {
		data[i].r = raw_data[i-1] ;
		data[i].i = 0.0 ;
		orig_data[i].r = raw_data[i-1] ;
		orig_data[i].i = 0.0 ;
	}
#ifdef DEBUG
	printf( "DEBUG: validate_data: copied data, begin FFT\n" ) ;
#endif // DEBUG

	/*
	 * Fast Fourier Transform
	 */
	FFT( data, NUM_DATA_PTS ) ;
#ifdef DEBUG
	printf( "DEBUG: validate_data: FFT done, begin power calcs and saving data\n" ) ;
#endif // DEBUG
	
	/*
	 * Calculate the Power and save to a csv file, if required
	 */
	char buffer[80] ;
	int n = 0 ;
	int data_fd = open( "/school/lab3data.csv", O_WRONLY | O_TRUNC | O_CREAT ) ;
	for ( i = 1 ; i <= NUM_DATA_PTS ; i++ ) {
		power[i] = data[i].r*data[i].r + data[i].i*data[i].i ;
		total_power += power[i] ;
		// save the data offline
		memset(buffer, 0x0, 80 ) ;
		n = sprintf( buffer, "%f,%f,%f,%f,%f\n", orig_data[i].r, orig_data[i].i, data[i].r, data[i].i, power[i] ) ;
		n = write( data_fd, buffer, n ) ;
#ifdef DEBUG
		printf( "Power (%5.2f), Data (%5.2f, %5.2f)\n", power[i], data[i].r, data[i].i ) ;
#endif // DEBUG
	}
#ifdef DEBUG
	printf( "DEBUG: validate_data: power calcs and saving data done, begin threshhold\n" ) ;
#endif // DEBUG

	// calculate the where our power should be. 95% should be in the buckets < (NUM_DATA_PTS/2 - 1)
	double temp_power = 0.0 ;
	for ( i = 1 ; i <= (NUM_DATA_PTS/2 -1); i++ ) {
		temp_power += power[i] ;
	}
	if ( temp_power >= ( 0.95*total_power) ) {
		printf( "Power is below threshhold. Good\n" ) ;
	}
	else {
		printf( "Power is NOT below threshhold. Bad\n" ) ;
	}

#ifdef DEBUG
	printf( "DEBUG: validate_data: threshhold done\n" ) ;
#endif // DEBUG

	return 0 ;
}

int init_adc( int adc_fd, int channel, int bit_resolution )
{
	int return_value = 0 ;

	// record the file descriptor for the adc
	_adc_fd = adc_fd ;

	/*
	 * If we need to change the ADC from it's defaults, do so now
	 */
	if (channel != 0) {
		switch(channel) {
			case 2:
				return_value = ioctl(_adc_fd, ADC_CHANNEL_2, ADC_CHANNEL_2) ;
				break ;
			case 3:
				return_value = ioctl(_adc_fd, ADC_CHANNEL_3, ADC_CHANNEL_3) ;
				break ;
			default:
				fprintf( stderr, "Incorrect channel (%d), for ADC. Only accept 2 or 3\n", channel ) ;
		}
	}
	if (bit_resolution != 0) {
		switch(bit_resolution ) {
			case 0:
				return_value = ioctl(_adc_fd, ADC_TEN_BIT, ADC_TEN_BIT ) ;
				break ;
			case 2:
				return_value = ioctl(_adc_fd, ADC_TWELVE_BIT, ADC_TWELVE_BIT ) ;
				break ;
			default:
				fprintf( stderr, "Incorrect bit_resoluion (%d), for ADC. Only accept 10 or 12\n", bit_resolution ) ;
		}
	}

	/*
	 * get the first data points
	 */
	memset( raw_data, 0x0, sizeof(uint32_t) * NUM_DATA_PTS ) ;
	memset( next_data, 0x0, sizeof(uint32_t) * NUM_DATA_PTS ) ;
	int i = 0 ;
	printf( "Acquiring first batch of data. Please stand by . . .\n" ) ;
	for ( i = 0 ; i < NUM_DATA_PTS ; i++ ) {
#ifdef DEBUG
		int temp = 0 ;
		temp = get_data() ;
		printf( "DEBUG: pt (%d), data from adc (%d)\n", i, temp ) ;
		raw_data[i] = temp ;
#else
		raw_data[i] = get_data() ;
#endif
		usleep(sleep_time);
	}

	printf( "Begining to validate the ADC, please stand by . . .\n" ) ;
	return_value = validate_data() ;
	printf( "Completed validating the ADC.\n" ) ;

	return 0 ;
}

float error_derivative( void )
{
	int32_t diff = 0 ;
	float answer = 0.0 ;
	diff = raw_data[NUM_DATA_PTS-1] - raw_data[NUM_DATA_PTS-1 - 2] ;

	answer = ((float)diff / 2.0) ;
#ifdef DEBUG
	printf( "eto (%d), et2 (%d), diff (%d), answer (%10.2f)\n", raw_data[NUM_DATA_PTS-1], raw_data[NUM_DATA_PTS-1 - 2] , diff, answer ) ;
#endif //DEBUG

	return answer ;
	
}

uint32_t error_integral( void )
{
	uint32_t sum = 0 ;
	uint32_t i = 0 ;

	for ( i = 0 ; i < NUM_DATA_PTS ; i++ ) {
		sum += raw_data[i] ;
	}
#ifdef DEBUG
	printf( "sum (%d)\n", sum ) ;
#endif // DEBUG

	return sum ;
}

uint32_t error( void )
{
	uint32_t new_data = 0 ;
	
	/*
	 * get the new data point
	 */
	new_data = get_data() ;

	/*
	 * Advance the buffer of previous data points
	 */
	memcpy(next_data, &raw_data[1], sizeof(uint32_t)*(NUM_DATA_PTS-1) ) ;
	next_data[NUM_DATA_PTS-1] = new_data ;
	memcpy(raw_data, next_data, NUM_DATA_PTS * sizeof(uint32_t) ) ;
#ifdef DEBUG
	int i = 0 ;
	for ( i = 0 ; i < NUM_DATA_PTS ; i++ ) {
		printf( "i (%d), raw_data (%d), next_data (%d)\n", i, raw_data[i], next_data[i] ) ;
	}
#endif // DEBUG

	return new_data ;

}
