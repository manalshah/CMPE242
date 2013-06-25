/*
 * ADC Verification Program for the Tiny6410 Board and ARM11.
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
 * This is a user level program
 */

#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <device_driver/matthew_adc.h>
#include "fft.h"

int main( int argc, char *argv[] )
{
	uint16_t bit_resolution = 0 ;
	uint16_t channel = 0 ;
	uint16_t data_pts = 16 ;
	uint16_t i = 0 ;
	uint16_t fd = 0 ;
	int opt = 0 ;
	double total_power = 0.0 ;
	long return_value = 0 ;

	/*
	 * Read the user input
	 */
	while ((opt = getopt(argc, argv, "c:b:n:")) != -1) {
		switch (opt) {
		case 'n': // number of data points
			data_pts = atoi(optarg) ;
			break ;
		case 'b': // bit resolution
			bit_resolution = atoi(optarg) ;
			break ;
		case 'c': // adc channel
			channel = atoi(optarg) ;
			break ;
		default:
			fprintf( stderr, "Usage: %s  [-n num_pts]\n", argv[0] ) ;
			exit(1) ;
		}
	}

	// now that we have the real data_pts, create the Complex array
	Complex data[data_pts+1] ;
	Complex orig_data[data_pts+1] ;
	double power[data_pts+1] ;

	// we don't use the zero'th element, set it to zero
	data[0].r = 0.0 ;
	data[0].i = 0.0 ;
	orig_data[0].r = 0.0 ;
	orig_data[0].i = 0.0 ;

	// open the driver
	fd = open("/dev/matthew_adc", 0);

	if (fd < 0) {
		perror("Could not open adc device");
		exit(1);
	}

	/*
	 * If we need to change the ADC from it's defaults, do so now
	 */
	if (channel != 0) {
		switch(channel) {
			case 2:
				return_value = ioctl(fd, CHANNEL_2, CHANNEL_2) ;
				break ;
			case 3:
				return_value = ioctl(fd, CHANNEL_3, CHANNEL_3) ;
				break ;
			default:
				fprintf( stderr, "Incorrect channel for ADC. Only accept 2 or 3\n" ) ;
		}
	}
	if (bit_resolution != 0) {
		switch(bit_resolution ) {
			case 0:
				return_value = ioctl(fd, TEN_BIT, TEN_BIT ) ;
				break ;
			case 2:
				return_value = ioctl(fd, TWELVE_BIT, TWELVE_BIT ) ;
				break ;
			default:
				fprintf( stderr, "Incorrect channel for ADC. Only accept 2 or 3\n" ) ;
		}
	}

	fprintf(stderr, "Will run for %d data points\n", data_pts );

	// read the data
	for( i = 1 ; i <= data_pts; i++ )  {
		char buffer[30];
		int len = 0 ;

		memset( buffer, 0x0, 30 ) ;
		len = read(fd, buffer, sizeof buffer -1);

		if (len > 0) {
			buffer[len] = '\0';
			int value = -1;
			sscanf(buffer, "%d", &value);
			printf("ADC Value: %d\n", value);
			data[i].r = value ;
			data[i].i = 0.0 ;
			orig_data[i].r = value ;
			orig_data[i].i = 0.0 ;
		} else {
			perror("read ADC device:");
			return 1;
		}
		usleep(100* 1000);
	}

	FFT( data, data_pts ) ;
	
	close(fd);

	// write the data to a CSV file
	char buffer[80] ;
	int n = 0 ;
	fd = open( "/school/lab2data.csv", O_WRONLY | O_TRUNC | O_CREAT ) ;
	for ( i = 1 ; i <= data_pts ; i++ ) {
		memset(buffer, 0x0, 80 ) ;
		n = sprintf( buffer, "%f,%f,%f,%f\n", orig_data[i].r, orig_data[i].i, data[i].r, data[i].i ) ;
		n = write( fd, buffer, n ) ;
		power[i] = data[i].r*data[i].r + data[i].i*data[i].i ;
		total_power += power[i] ;
		//printf( "Power (%5.2f), Data (%5.2f, %5.2f)\n", power[i], data[i].r, data[i].i ) ;
	}

	// calculate the where our power should be. 95% should be in the buckets < (data_pts/2 - 1)
	double temp_power = 0.0 ;
	for ( i = 1 ; i <= (data_pts/2 -1); i++ ) {
		temp_power += power[i] ;
	}
	if ( temp_power >= ( 0.95*total_power) ) {
		printf( "Power is below threshhold. Good\n" ) ;
	}
	else {
		printf( "Power is NOT below threshhold. Bad\n" ) ;
	}


	return 0 ;
}
