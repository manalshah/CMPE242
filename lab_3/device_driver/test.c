#include "matthew_leds.h"
#include <stdio.h>

int main( int argc, char *argv[] )
{
	printf( "ON (%d)\n", TURN_ON_LED ) ;
	printf( "OFF (%d)\n", TURN_OFF_LED ) ;
	printf( "READ (%d)\n", READ_SWITCH ) ;

	return 0 ;
}
