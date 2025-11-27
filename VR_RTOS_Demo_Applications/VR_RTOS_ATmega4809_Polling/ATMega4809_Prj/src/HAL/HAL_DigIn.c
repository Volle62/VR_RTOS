/**********************************************************************\
*
* file: HAL_DigIn.c
*
* Project : Demo programm VR_RTOS
*
* Description: Digital Inputs for Demo programm VR_RTOS
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

#include "HW/HW_DigIn.h"
#include "HAL/HAL_DigIn.h"

//*************************
// extern variables *******
//*************************

//*************************
// extern funktions *****
//*************************
extern bool Is_ShellMode_Test(void); // Task_Shell.c

//*************************
// static variables ********
//*************************
#include "HAL_Config/HAL_DigIn_Cfg.inc"
static uint8_t s_MaxDigIn=0;

void HAL_Init_DigIns(void)
{
    s_MaxDigIn = getNumberOfDigIns();

	for (uint8_t i=0; i<s_MaxDigIn; i++)
	{
    	HW_Init_DigIn(DigIn_Detail[i].Port, DigIn_Detail[i].Pin);
	}
}

bool HAL_Get_DigIn(uint8_t digIn_no)
{
	bool state=false;
	
	if (digIn_no >= s_MaxDigIn)
		return (0);
		
    if (!Is_ShellMode_Test())
    {
        DigIn_Detail[digIn_no].value = HW_Get_DigIn(DigIn_Detail[digIn_no].Port, DigIn_Detail[digIn_no].Pin);
    }

    state = DigIn_Detail[digIn_no].value;
	
 	
	if (DigIn_Detail[digIn_no].invert)
		state = !state;

	return (state);
}

char *HAL_Get_DigIn_Name(uint8_t digIn_no)
{
	if (digIn_no >= s_MaxDigIn)
		return (NULL);
	
	return(DigIn_Detail[digIn_no].DigIn_Name);
}

uint8_t HAL_Get_DigIn_No(char *DigIn_Name)
{
	for (uint8_t i=0; i<s_MaxDigIn; i++)
	{
		if (strcmp(DigIn_Detail[i].DigIn_Name, DigIn_Name) == 0)
			return (i);
	}
	
	return (0);
}

uint8_t HAL_get_MaxDigIn()
{
	return (s_MaxDigIn);
}

uint16_t getNumberOfDigIns(void)
{
    uint16_t NumberOfDigIns;

    NumberOfDigIns = sizeof(DigIn_Detail) / sizeof(DigIn_Detail[0]);

    return (NumberOfDigIns);
}


// Start functions only for Task_Shell !!!
void Set_DigIn_Test(uint8_t digIn_no, bool digVal)
{
    bool state=digVal;

	if (DigIn_Detail[digIn_no].invert)
	    state = !state;

    DigIn_Detail[digIn_no].value = state;
}
// End functions only for Task_Shell !!!
