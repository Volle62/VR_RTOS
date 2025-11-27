/**********************************************************************\
*
* file: HAL_PwmOut.c
*
* Project : Demo programm VR_RTOS
*
* Description: PWM Outputs for Demo programm VR_RTOS
*
* Datum: 20.10.2025       Author: Volker Rumm
*
\**********************************************************************/

//*************************
// includes ***************
//*************************
#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "HW/HW_DigOut.h"
#include "HAL/HAL_PwmOut.h"

//*************************
// extern variables *******
//*************************

//*************************
// extern funktions *******
//*************************
extern bool Is_ShellMode_Manu(void);  // Task_Shell.c

//*************************
// static variables *******
//*************************
#include "HAL_Config/HAL_PwmOut_Cfg.inc"
static uint8_t s_MaxPwmOut=0;

// implementation------------------------------------------
void HAL_Init_PwmOuts(void)
{
	s_MaxPwmOut = getNumberOfPwmOuts();
	
	for (uint8_t i=0; i<s_MaxPwmOut; i++)
	{
		HW_Init_DigOut(PwmOut_Detail[i].Port, PwmOut_Detail[i].Pin);
        HW_SetRes_DigOut(PwmOut_Detail[i].Port, PwmOut_Detail[i].Pin, 0);
        PwmOut_Detail[i].Value = 0;
	}
}

void HAL_Set_PwmOut_Value(uint8_t pwmOut_no, uint8_t value)
{
    if (pwmOut_no >= s_MaxPwmOut)
    return;
    
    if (!Is_ShellMode_Manu())
    {
        if (PwmOut_Detail[pwmOut_no].Invert)
        {
            PwmOut_Detail[pwmOut_no].Value = PWM_MAX_VALUE - value;
        }
        else
        {
            PwmOut_Detail[pwmOut_no].Value = value;
        }
    }
}

void HAL_Set_PwmOut_State(uint8_t pwmOut_no, bool state)
{
    if (pwmOut_no >= s_MaxPwmOut)
        return;
    
    if (!Is_ShellMode_Manu())
    {
        if (PwmOut_Detail[pwmOut_no].Invert)
	    	state = !state;
        HW_SetRes_DigOut(PwmOut_Detail[pwmOut_no].Port, PwmOut_Detail[pwmOut_no].Pin, state);
    }
}

uint8_t HAL_Get_PwmOut_Value(uint8_t pwmOut_no)
{
    uint8_t value;

	if (pwmOut_no >= s_MaxPwmOut)
		return (0);

	if (PwmOut_Detail[pwmOut_no].Invert)
    {
        value = PWM_MAX_VALUE - PwmOut_Detail[pwmOut_no].Value;
    }
    else
    {
        value = PwmOut_Detail[pwmOut_no].Value;
    }

	return (value);
}

bool HAL_Get_PwmOut_State(uint8_t pwmOut_no)
{
	if (pwmOut_no >= s_MaxPwmOut)
	    return (0);
	
	return (HW_Get_DigOut(PwmOut_Detail[pwmOut_no].Port, PwmOut_Detail[pwmOut_no].Pin));
}

char *HAL_Get_PwmOut_Name(uint8_t pwmOut_no)
{
	if (pwmOut_no >= s_MaxPwmOut)
		return (NULL);
	
	return(PwmOut_Detail[pwmOut_no].PwmOut_Name);
}

uint8_t HAL_Get_PwmOut_No(char *pwmOut_Name)
{
	for (uint8_t i=0; i<s_MaxPwmOut; i++)
	{
		if (strcmp(PwmOut_Detail[i].PwmOut_Name, pwmOut_Name) == 0)
			return (i);
	}
	
	return (0);
}

uint8_t HAL_get_MaxPwmOut()
{
	return (s_MaxPwmOut);
}

uint16_t getNumberOfPwmOuts(void)
{
    uint16_t NumberOfPwmOuts;

    NumberOfPwmOuts = sizeof(PwmOut_Detail) / sizeof(PwmOut_Detail[0]);

    return (NumberOfPwmOuts);
}

// Start functions only for Task_Shell !!!
void Set_PwmOut_Manu(uint8_t pwmOut_no, uint8_t value)
{
    PwmOut_Detail[pwmOut_no].Value = value;
}
// End functions only for Task_Shell !!!

