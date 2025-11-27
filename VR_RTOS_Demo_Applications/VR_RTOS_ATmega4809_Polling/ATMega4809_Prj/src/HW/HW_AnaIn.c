#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "HW/HW_AnaIn.h"

void HW_Init_AnaIn(uint8_t Ch)
{
    static bool First_Call=true;

    if (First_Call)
    {
        ADC0.CTRLC = ADC_PRESC_DIV4_gc | ADC_REFSEL_INTREF_gc; // CLK_PER divide by 4 or internal reference
    }

    switch (Ch)
    {
        case 0 : // disable digital input buffer
            PORTD.PIN0CTRL &= ~PORT_ISC_gm;
            PORTD.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN0CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 1 : // disable digital input buffer
            PORTD.PIN1CTRL &= ~PORT_ISC_gm;
            PORTD.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN1CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 2 : // disable digital input buffer
            PORTD.PIN2CTRL &= ~PORT_ISC_gm;
            PORTD.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN2CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 3 : // disable digital input buffer
            PORTD.PIN3CTRL &= ~PORT_ISC_gm;
            PORTD.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN3CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 4 : // disable digital input buffer
            PORTD.PIN4CTRL &= ~PORT_ISC_gm;
            PORTD.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN4CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 5 : // disable digital input buffer
            PORTD.PIN5CTRL &= ~PORT_ISC_gm;
            PORTD.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN5CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 6 : // disable digital input buffer
            PORTD.PIN6CTRL &= ~PORT_ISC_gm;
            PORTD.PIN6CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN6CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 7 : // disable digital input buffer
            PORTD.PIN7CTRL &= ~PORT_ISC_gm;
            PORTD.PIN7CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTD.PIN7CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 8 : // disable digital input buffer
            PORTE.PIN0CTRL &= ~PORT_ISC_gm;
            PORTE.PIN0CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTE.PIN0CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 9 : // disable digital input buffer
            PORTE.PIN1CTRL &= ~PORT_ISC_gm;
            PORTE.PIN1CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTE.PIN1CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 10 : // disable digital input buffer
            PORTE.PIN2CTRL &= ~PORT_ISC_gm;
            PORTE.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTE.PIN2CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 11 : // disable digital input buffer
            PORTE.PIN3CTRL &= ~PORT_ISC_gm;
            PORTE.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTE.PIN3CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 12 : // disable digital input buffer
            PORTF.PIN2CTRL &= ~PORT_ISC_gm;
            PORTF.PIN2CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTF.PIN2CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 13 : // disable digital input buffer
            PORTF.PIN3CTRL &= ~PORT_ISC_gm;
            PORTF.PIN3CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTF.PIN3CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 14 : // disable digital input buffer
            PORTF.PIN4CTRL &= ~PORT_ISC_gm;
            PORTF.PIN4CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTF.PIN4CTRL &= ~PORT_PULLUPEN_bm;
            break;

        case 15 : // disable digital input buffer
            PORTF.PIN5CTRL &= ~PORT_ISC_gm;
            PORTF.PIN5CTRL |= PORT_ISC_INPUT_DISABLE_gc;
            // disable pull-up resistor
            PORTF.PIN5CTRL &= ~PORT_PULLUPEN_bm;
            break;

       default:
            break;
    }

    if (First_Call)
    {
        First_Call = false;
        ADC0.CTRLA = ADC_ENABLE_bm | ADC_RESSEL_10BIT_gc; // ADC0 enable and use 10-bit mode
    }
}

void HW_Start_AnaIn(uint8_t Ch)
{
    switch (Ch)
    {
        case 0:  ADC0.MUXPOS = ADC_MUXPOS_AIN0_gc; break;
        case 1:  ADC0.MUXPOS = ADC_MUXPOS_AIN1_gc; break;
        case 2:  ADC0.MUXPOS = ADC_MUXPOS_AIN2_gc; break;
        case 3:  ADC0.MUXPOS = ADC_MUXPOS_AIN3_gc; break;
        case 4:  ADC0.MUXPOS = ADC_MUXPOS_AIN4_gc; break;
        case 5:  ADC0.MUXPOS = ADC_MUXPOS_AIN5_gc; break;
        case 6:  ADC0.MUXPOS = ADC_MUXPOS_AIN6_gc; break;
        case 7:  ADC0.MUXPOS = ADC_MUXPOS_AIN7_gc; break;
        case 8:  ADC0.MUXPOS = ADC_MUXPOS_AIN8_gc; break;
        case 9:  ADC0.MUXPOS = ADC_MUXPOS_AIN9_gc; break;
        case 10: ADC0.MUXPOS = ADC_MUXPOS_AIN10_gc; break;
        case 11: ADC0.MUXPOS = ADC_MUXPOS_AIN11_gc; break;
        case 12: ADC0.MUXPOS = ADC_MUXPOS_AIN12_gc; break;
        case 13: ADC0.MUXPOS = ADC_MUXPOS_AIN13_gc; break;
        case 14: ADC0.MUXPOS = ADC_MUXPOS_AIN14_gc; break;
        case 15: ADC0.MUXPOS = ADC_MUXPOS_AIN15_gc; break;
    }

    // start conversion
    ADC0.COMMAND = ADC_STCONV_bm;
}

bool HW_Is_AnaIn_Ready(void) // is the ADC conversion done ?
{
    bool done;

    done = (ADC0.INTFLAGS & ADC_RESRDY_bm) > 0;
    
    return done;
}

uint16_t HW_Read_AnaIn(void)
{
    // clear the interrupt flag by writing 1
    ADC0.INTFLAGS = ADC_RESRDY_bm;

    return (ADC0.RES);
}


