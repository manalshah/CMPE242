/*
 * CMPE242 Lab3. PID Controller. Use GPIO, ADC, and PWM.
 *
 * Written by
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include <device_driver/matthew_gpio.h>
#include <device_driver/matthew_adc.h>
#include <device_driver/matthew_pwm.h>

#include "adc.h"

static const char * GPIO_FN = "/dev/matthew_gpio";
static const char * ADC_FN = "/dev/matthew_adc";
static const char * PWM_FN = "/dev/matthew_pwm";


/*
 * used for determining Kp, Ki, and Kd.
 * The max value from the ADC is 1024. Therefore the initial max values will be this max value
 */
#define MAX_ERROR 1024.0
float E_MAX = MAX_ERROR ;
float E_INT_MAX = MAX_ERROR ;
float E_D_MAX = MAX_ERROR ;
static const int TOLERENCE = 0.05 ; // if we're within 5%, we're good.
static const float PWM_MAX = 1000.0 ;
static const float one_third = 1.0/3.0 ;

int main( int argc, char *argv[] )
{
	static const uint32_t half_sec = 500 * 1000 ;
	int opt = 0 ; // command line arguments
	uint32_t desired_position = 0 ; // user supplied desired pos
	uint32_t desired_direction = 0 ; // user supplied direction
	bool reached_position = false ; // have we reached the position
	bool debug = false ; // reduced the freq of the PWM for human viewing
	bool fwd = false ; // move motor forward
	bool rev = false ; // move motor backward
	int adc_fd = 0 ; // file descriptor for the the adc
	int pwm_fd = 0 ; // file descriptor for the the pwm
	int gpio_fd = 0 ; // file descriptor for the the gpio
	int return_value = 0 ; // return value from the device drivers

	/*
	 * Read the user input on motor speed and direction
	 */
	while ( (opt = getopt(argc, argv, "p:frd")) != -1) {
		switch (opt) {
		case 'p': // position
			desired_position = atoi( optarg ) ;
			break ;
		case 'd': //debug
			debug = true ;
			break ;
		case 'f': // foward
			fwd = true ;
			break ;
		case 'r': // rev
			rev = true ;
			break ;
		default:
			fprintf( stderr, "Usage: %s [-p positon][-f | -r][-d]\n", argv[0] ) ;
			exit(1) ;
		}
	}

	/*
	 * Check the arguments
	 */
	if ( ( fwd && rev ) || !(fwd || rev) ) {
		printf( "Need ONE direction please. Use -f for forward OR -r for reverse.\n" ) ;
		exit(2) ;
	}
	// direction good, set the command
	desired_direction = ( fwd ? GPIO_FORWARD : GPIO_REVERSE ) ;

	if ( 0 == desired_position ) {
		printf( "Need a position. Use -p <postion>.\n" ) ;
		exit(3) ;
	}

	printf( "The desired_position is (%d) and the direction is (%s)\n", desired_position, (fwd?"forward":"reverse") ) ;

	/*
	 * Open the deivce drivers. There should be 3 of them
	 * 1) ADC - Read the motor positon error
	 * 2) PWM - Command motor speed and by extension the position
	 * 3) GPIO - Motor direction
	 */
	
	// ADC
	adc_fd = open(ADC_FN, 0) ;
	if (adc_fd < 0) {
		perror("Could not open adc device");
		exit(4);
	}

	// PWM
	pwm_fd = open(PWM_FN, 0) ;
	if (pwm_fd < 0) {
		perror("Could not open pwm device");
		exit(4);
	}

	// GPIO
	gpio_fd = open(GPIO_FN, 0) ;
	if (gpio_fd < 0) {
		perror("Could not open gpio device");
		exit(4);
	}

	/*
	 * Validate the ADC. Before we begin to move the motor ensure that the ADC can read
	 * the data correctly. Collect 1024 data points to verify
	 */
	return_value = init_adc( adc_fd, 0, 0 ) ;
	 

	/*
	 * The BIG loop. Until we reach the position, control the motor
	 */
	int pos_error = 0 ; // position error
	int total_error = 0 ; // integral (sum) of position error
	float error_slope = 0.0 ; // derivative (slope) of position error
	float Kp = 0.0 ;
	float Ki = 0.0 ;
	float Kd = 0.0 ;
	long freq = 0 ;
	while ( false == reached_position ) {
		/*
		 * Read the error.
		 */
		pos_error = error() ;
		if ( (0 == pos_error) || (TOLERENCE*MAX_ERROR >= (float)pos_error) ) {
			printf( "Reached the position. Error (%d)\n", pos_error ) ;
			reached_position = true ;
			continue ;
		}

		/*
		 * PID Controller. Run the error through the controller.
		 */
		// P
		if (pos_error > E_MAX) {
			E_MAX = pos_error ;
		}
		Kp = one_third*(pos_error/E_MAX)*PWM_MAX ;

		// I
		total_error = error_integral() ;
		if (total_error > E_INT_MAX) {
			E_INT_MAX = total_error ;
		}
		Ki = one_third*(total_error/E_INT_MAX)*PWM_MAX ;
#ifdef DEBUG
		printf( "total_error = (%d)\n", total_error ) ;
#endif // DEBUG

		// D
		error_slope = error_derivative() ;
		if (error_slope > E_D_MAX) {
			E_D_MAX = error_slope ;
		}
		Kd = one_third*(error_slope/E_D_MAX)*PWM_MAX ;
#ifdef DEBUG
		printf( "error_slope = (%10.2f)\n", error_slope ) ;
#endif // DEBUG

		freq = (long)(Kp + Kd + Ki) ;

		// will need to change direction if we've overshot
		// but we'll get to that later . . .
		if (freq < 0 ) {
			freq = freq * -1 ;
			printf( "Overshoot\n" ) ;
		}

		// if there is no command to the motor, then we're here.
		else if ( 0 == freq ) {
			reached_position = true ;
			printf( "freq == 0\n" ) ;
			continue ;
		}

		printf( "freq (%ld), Kp (%10.2f), Ki (%10.2f), Kd (%10.2f)\n", freq, Kp, Ki, Kd ) ;

		/*
		 * Set the motor speed.
		 */
		return_value = ioctl(pwm_fd, PWM_IOCTL_SET_FREQ, freq) ;
		if ( return_value != 0 ) {
			perror( "Error when setting motor speed" ) ;
			break ;
		}

		/*
		 * Set the motor direction
		 */
		return_value = ioctl( gpio_fd, desired_direction ) ;
		if ( return_value != 0 ) {
			perror( "Error when setting direction" ) ;
			break ;
		}

#ifdef TESTING
		usleep( half_sec ) ;
		printf( "Debug reached position\n" ) ;
		reached_position = true ;
#endif // TESTING
	}

	/*
	 * We've reached the desired position
	 * Stop the motor
	 */
	if ( reached_position ) {
		// turn off direction
		return_value = ioctl( gpio_fd, GPIO_STOP ) ;
		if ( return_value != 0 ) {
			perror( "Error when stopping direction" ) ;
		}

		// turn off speed/power
		return_value = ioctl( pwm_fd, PWM_IOCTL_STOP);
		if ( return_value != 0 ) {
			perror("Error when stopping motor" ) ;
		}

	 	/*
		 * Turn on the 'complete' led to let user know we're there.
	 	 * Turn it on at 2Hz for 5 seconds.
		 */
		uint32_t num_sec = 0 ;
		while ( num_sec < 5 ) {
			return_value = ioctl( gpio_fd, GPIO_COMPLETE_ON ) ;
			if ( return_value != 0 ) {
				perror( "Error when turing on complete" ) ;
				break ;
			}

			return_value = usleep( half_sec ) ;
			if ( return_value != 0 ) {
				perror( "Error when micro sleeping" ) ;
				break ;
			}

			return_value = ioctl( gpio_fd, GPIO_COMPLETE_OFF ) ;
			if ( return_value != 0 ) {
				perror( "Error when turing on complete" ) ;
				break ;
			}

			return_value = usleep( half_sec ) ;
			if ( return_value != 0 ) {
				perror( "Error when micro sleeping" ) ;
				break ;
			}

			num_sec++ ;
		}
		
	}


	/*
	 * Regardless of how we got here, reset the direction
	 * and turn everything off.
	 */
	return_value = ioctl( gpio_fd, GPIO_RESET ) ;
	if ( return_value != 0 ) {
		perror( "Error when reseting direction" ) ;
	}
	return_value = ioctl( pwm_fd, PWM_RESET ) ;
	if ( return_value != 0 ) {
		perror( "Error when reseting motor" ) ;
	}

	return 0 ;
}
