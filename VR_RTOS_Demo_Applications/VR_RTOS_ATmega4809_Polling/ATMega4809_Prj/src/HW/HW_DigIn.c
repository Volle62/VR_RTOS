#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "HW/HW_DigIn.h"

void HW_Init_DigIn(uint8_t port, uint8_t pin)
{
	switch (port)
	{
    	case 'B' : PORTB_set_pin_dir(pin, PORT_DIR_IN); break;
    	case 'C' : PORTC_set_pin_dir(pin, PORT_DIR_IN); break;
    	case 'D' : PORTD_set_pin_dir(pin, PORT_DIR_IN); break;
	}
}

bool HW_Get_DigIn(uint8_t port, uint8_t pin)
{
	bool state=false;
	
	switch (port)
	{
		case 'B' : state = PORTB_get_pin_level(pin); break;
		case 'C' : state = PORTC_get_pin_level(pin); break;
		case 'D' : state = PORTD_get_pin_level(pin); break;
	}
	
	return (state);
}
