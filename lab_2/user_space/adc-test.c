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
#include <inttypes.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fft.h"

int main( int argc, char *argv[] )
{
	uint16_t N = 16 ;
	uint16_t i = 0 ;
	uint16_t fd = 0 ;
	int opt = 0 ;
	double total_power = 0.0 ;

	/*
	 * Read the user input
	 */
	while ((opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
		case 'n':
			N = atoi(optarg) ;
			break ;
		default:
			fprintf( stderr, "Usage: %s  [-n num_pts]\n", argv[0] ) ;
			exit(1) ;
		}
	}

	// now that we have the real N, create the Complex array
	Complex data[N+1] ;
	Complex orig_data[N+1] ;
	double power[N+1] ;

	// we don't use the zero'th element, set it to zero
	data[0].r = 0.0 ;
	data[0].i = 0.0 ;
	orig_data[0].r = 0.0 ;
	orig_data[0].i = 0.0 ;

	// open the driver
	fd = open("/dev/mini6410_adc", 0);

	if (fd < 0) {
		perror("Could not open adc device");
		exit(1);
	}

	fprintf(stderr, "Will run for %d data points\n", N );

	// read the data
	for( i = 1 ; i <= N; i++ )  {
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
		usleep(500* 1000);
	}

	FFT( data, N ) ;
	
	close(fd);

	// write the data to a CSV file
	char buffer[80] ;
	int n = 0 ;
	fd = open( "/school/lab2data.csv", O_WRONLY | O_TRUNC | O_CREAT ) ;
	for ( i = 1 ; i <= N ; i++ ) {
		memset(buffer, 0x0, 80 ) ;
		n = sprintf( buffer, "%f,%f,%f,%f\n", orig_data[i].r, orig_data[i].i, data[i].r, data[i].i ) ;
		n = write( fd, buffer, n ) ;
		power[i] = data[i].r*data[i].r + data[i].i*data[i].i ;
		total_power += power[i] ;
		printf( "Power (%5.2f), Data (%5.2f, %5.2f)\n", power[i], data[i].r, data[i].i ) ;
	}

	// calculate the where our power should be. 95% should be in the buckets < (N/2 - 1)
	double temp_power = 0.0 ;
	for ( i = 1 ; i <= (N/2 -1); i++ ) {
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
