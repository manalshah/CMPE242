/*
 * Simple I/O Device Driver for the Tiny6410 Board
 * Matthew Lopez
 * CMPE 242
 * Spring 2013
 * San Jose State University
 *
 * This is the user level program
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

 // required to ensure consistent commands between driver and user space
#include <device_driver/matthew_leds.h>

int main(int argc, char **argv)
{
	int read_switch = -1 ;
	int led_cmd = -1 ;
	int opt = 0 ;
	int fd = 0 ;

	/*
	 * Read the user input
	 */
	while ((opt = getopt(argc, argv, "rl:")) != -1) {
		switch (opt) {
		case 'r': // read the status of the switch
			read_switch = READ_SWITCH ;
			break ;
		case 'l':
			switch (atoi(optarg)) {
			case 0:
				led_cmd = TURN_OFF_LED ;
				break ;
			case 1:
				led_cmd = TURN_ON_LED ;
				break ;
			default:
				fprintf( stderr, "Usage: %s [-r] [-l state] \nWhere state = (0|1)\n", argv[0] ) ;
				exit(2) ;
			}
			break ;
		default:
			fprintf( stderr, "Usage: %s [-r] [-l state] \nWhere state = (0|1)\n", argv[0] ) ;
			exit(1) ;
		}
	}

	printf( "read_switch (%d), led_cmd (%d)\n", read_switch, led_cmd ) ;

	fd = open("/dev/matthew_leds", 0);
	if (fd < 0) {
		printf( "/dev/matthew_leds, didn't work. Trying /dev/matthew_leds0\n" ) ;
		fd = open("/dev/matthew_leds0", 0);
	}
	if (fd < 0) {
		perror("open device leds");
		exit(1);
	}

	int return_value = 0 ;
	if (led_cmd != -1) {
		return_value = ioctl(fd, led_cmd, led_cmd);
		if (return_value < 0 ) {
			perror( "Error when attempting to commanding LED" ) ;
			fprintf( stderr, "Error commanding LED: (%d)\n", return_value ) ;
		}
	}

	if (read_switch != -1) {
		return_value = ioctl(fd, read_switch) ;
		if (return_value < 0 ) {
			perror( "Error when attempting to read switch" ) ;
		}
		printf( "The switch value is (%d)\n", return_value ) ;
	}
	close(fd);

	return 0 ;
}
