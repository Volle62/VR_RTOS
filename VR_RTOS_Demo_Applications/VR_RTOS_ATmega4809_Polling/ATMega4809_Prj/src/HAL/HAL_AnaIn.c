/**********************************************************************\
*
* file: HAL_AnaIn.c
*
* Project : Demo programm VR_RTOS
*
* Description: Analog Inputs for Demo programm VR_RTOS
*
* Datum: 01.02.2025       Author: Volker Rumm
*
\**********************************************************************/

#include <atmel_start.h>

//*************************
// includes ***************
//*************************
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "HW/HW_AnaIn.h"
#include "HAL/HAL_AnaIn.h"

//*************************
// extern variables *******
//*************************

//*************************
// extern funktions *******
//*************************
extern bool Is_ShellMode_Test(void);  // Task_Shell.c

//*************************
// static variables *******
//*************************
#include "HAL_Config/HAL_AnaIn_Cfg.inc"
static uint8_t s_MaxAnaIn=0;
static uint8_t s_ActiveChannel=0;

void HAL_Init_AnaIns(void)
{
    s_MaxAnaIn = getNumberOfAnaIns();

    for (uint8_t i = 0; i < s_MaxAnaIn; i++)
        HW_Init_AnaIn(AnaIn_Detail[i].Channel);
}

void HAL_Start_AnaIn(uint8_t Ch)
{
    HW_Start_AnaIn(Ch);
    s_ActiveChannel = Ch;
}

bool HAL_Is_AnaIn_Ready(void) // is the ADC conversion done ?
{
    return (HW_Is_AnaIn_Ready());
}

uint16_t HAL_Read_AnaIn(void)
{
	uint8_t AnaIn_Value=0;
	
    if (!Is_ShellMode_Test())
    {
        AnaIn_Detail[s_ActiveChannel].value = HW_Read_AnaIn();
    }

    AnaIn_Value = AnaIn_Detail[s_ActiveChannel].value;
	
	return (AnaIn_Value);
}

uint16_t HAL_Get_AnaIn_Value(uint8_t AnaIn_no)
{
    return (AnaIn_Detail[AnaIn_no].value);
}

char *HAL_Get_AnaIn_Name(uint8_t AnaIn_no)
{
	if (AnaIn_no >= s_MaxAnaIn)
		return (NULL);
	
	return(AnaIn_Detail[AnaIn_no].AnaIn_Name);
}

uint8_t HAL_Get_AnaIn_No(char *AnaIn_Name)
{
	for (uint8_t i=0; i<s_MaxAnaIn; i++)
	{
		if (strcmp(AnaIn_Detail[i].AnaIn_Name, AnaIn_Name) == 0)
			return (i);
	}
	
	return (0);
}

uint8_t HAL_get_MaxAnaIn(void)
{
	return (s_MaxAnaIn);
}

uint16_t getNumberOfAnaIns(void)
{
    uint16_t NumberOfAnaIns;

    NumberOfAnaIns = sizeof(AnaIn_Detail) / sizeof(AnaIn_Detail[0]);

    return (NumberOfAnaIns);
}


// Start functions only for Task_Shell !!!
void Set_AnaIn_Test(uint8_t AnaIn_no, uint16_t AnaVal)
{
    AnaIn_Detail[AnaIn_no].value = AnaVal;
}
// End functions only for Task_Shell !!!
