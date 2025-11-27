/**********************************************************************\
*
* file: HAL_DigOut.c
*
* Project : Demo programm VR_RTOS
*
* Description: Digital Outputs for Demo programm VR_RTOS
*
* Datum: 01.02.2025       Author: Volker Rumm
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
#include "HAL/HAL_DigOut.h"


//*************************
// extern variables *******
//*************************

//*************************
// extern funktions *******
//*************************
extern bool     Is_ShellMode_Manu(void);  // Task_Shell.c

//*************************
// static variables *******
//*************************
#include "HAL_Config/HAL_DigOut_Cfg.inc"
static uint8_t s_MaxDigOut=0;

void HAL_Init_DigOuts(void)
{
	s_MaxDigOut = getNumberOfDigOuts();
	
	for (uint8_t i=0; i<s_MaxDigOut; i++)
	{
		HW_Init_DigOut(DigOut_Detail[i].Port, DigOut_Detail[i].Pin);
	}
}

void HAL_Set_DigOut(uint8_t digOut_no)
{
	if (digOut_no >= s_MaxDigOut)
		return; 
		
	if (!Is_ShellMode_Manu())
    {
        bool state = !DigOut_Detail[digOut_no].invert;
		HW_SetRes_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin, state);
    }
}

void HAL_Reset_DigOut(uint8_t digOut_no)
{
	if (digOut_no >= s_MaxDigOut)
		return;
	
	if (!Is_ShellMode_Manu())
	{
	    bool state = DigOut_Detail[digOut_no].invert;
	    HW_SetRes_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin, state);
    }
}

void HAL_SetRes_DigOut(uint8_t digOut_no, bool state)
{
	if (digOut_no >= s_MaxDigOut)
		return;
		
	if (!Is_ShellMode_Manu())
	{
    	if (DigOut_Detail[digOut_no].invert)
	    	state = !state;
		HW_SetRes_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin, state);
    }
}

void HAL_Toggle_DigOut(uint8_t digOut_no)
{
	if (digOut_no >= s_MaxDigOut)
		return;
		
	if (!Is_ShellMode_Manu())
	{
        HW_Toggle_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin);
    }
}

bool HAL_Get_DigOut(uint8_t digOut_no)
{
	bool state=false;
	
	if (digOut_no >= s_MaxDigOut)
		return (0);
	
	state = HW_Get_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin);
	
	if (DigOut_Detail[digOut_no].invert)
		state = !state;
	
	return (state);
}

char *HAL_Get_DigOut_Name(uint8_t digOut_no)
{
	if (digOut_no >= s_MaxDigOut)
		return (NULL);
	
	return(DigOut_Detail[digOut_no].DigOut_Name);
}

uint8_t HAL_Get_DigOut_No(char *DigOut_Name)
{
	for (uint8_t i=0; i<s_MaxDigOut; i++)
	{
		if (strcmp(DigOut_Detail[i].DigOut_Name, DigOut_Name) == 0)
			return (i);
	}
	
	return (0);
}

uint8_t HAL_get_MaxDigOut()
{
	return (s_MaxDigOut);
}

uint16_t getNumberOfDigOuts(void)
{
    uint16_t NumberOfDigOuts;

    NumberOfDigOuts = sizeof(DigOut_Detail) / sizeof(DigOut_Detail[0]);

    return (NumberOfDigOuts);
}

// Start functions only for Task_Shell !!!
void Set_DigOut_Manu(uint8_t digOut_no)
{
    bool state = !DigOut_Detail[digOut_no].invert;
    HW_SetRes_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin, state);
}

void Reset_DigOut_Manu(uint8_t digOut_no)
{
    bool state = !DigOut_Detail[digOut_no].invert;
    HW_SetRes_DigOut(DigOut_Detail[digOut_no].Port, DigOut_Detail[digOut_no].Pin, !state);
}

void Toggle_DigOut_Manu(uint8_t digOut_no)
{
    if (HAL_Get_DigOut(digOut_no))
        Reset_DigOut_Manu(digOut_no);
    else
        Set_DigOut_Manu(digOut_no);
}
// End functions only for Task_Shell !!!

