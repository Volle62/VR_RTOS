#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "HW/HW_DigOut.h"


void HW_Init_DigOut(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case 'A' : PORTA_set_pin_dir(pin, PORT_DIR_OUT); break;
		case 'B' : PORTB_set_pin_dir(pin, PORT_DIR_OUT); break;
		case 'C' : PORTC_set_pin_dir(pin, PORT_DIR_OUT); break;
		case 'D' : PORTD_set_pin_dir(pin, PORT_DIR_OUT); break;
		case 'E' : PORTE_set_pin_dir(pin, PORT_DIR_OUT); break;
		case 'F' : PORTF_set_pin_dir(pin, PORT_DIR_OUT); break;
	}
}

void HW_SetRes_DigOut(uint8_t port, uint8_t pin, bool state)
{
	switch (port)
	{
		case 'A' : PORTA_set_pin_level(pin, state); break;
		case 'B' : PORTB_set_pin_level(pin, state); break;
		case 'C' : PORTC_set_pin_level(pin, state); break;
		case 'D' : PORTD_set_pin_level(pin, state); break;
		case 'E' : PORTE_set_pin_level(pin, state); break;
		case 'F' : PORTF_set_pin_level(pin, state); break;
	}
}

void HW_Toggle_DigOut(uint8_t port, uint8_t pin)
{
	switch (port)
	{
		case 'A' : PORTA_toggle_pin_level(pin); break;
		case 'B' : PORTB_toggle_pin_level(pin); break;
		case 'C' : PORTC_toggle_pin_level(pin); break;
		case 'D' : PORTD_toggle_pin_level(pin); break;
		case 'E' : PORTE_toggle_pin_level(pin); break;
		case 'F' : PORTF_toggle_pin_level(pin); break;
	}
}

bool HW_Get_DigOut(uint8_t port, uint8_t pin)
{
	bool state=false;
	
	switch (port)
	{
		case 'A' : state = PORTA_get_pin_level(pin); break;
		case 'B' : state = PORTB_get_pin_level(pin); break;
		case 'C' : state = PORTC_get_pin_level(pin); break;
		case 'D' : state = PORTD_get_pin_level(pin); break;
		case 'E' : state = PORTE_get_pin_level(pin); break;
		case 'F' : state = PORTF_get_pin_level(pin); break;
	}
	
	return (state);
}
