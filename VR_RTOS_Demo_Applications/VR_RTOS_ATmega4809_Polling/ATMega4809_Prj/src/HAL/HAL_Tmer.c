#include "HAL/HAL_Timer.h"
#include "HW/HW_timer.h"

#include <compiler.h>

uint16_t HAL_OS_Tick_ms = HAL_OS_TICK_MS;
uint16_t HAL_OS_Tick_MaxCounter = HAL_OS_TICK_MAX_COUNTER;
uint16_t HAL_Statis_Tick_MaxCounter = HAL_STATIS_TICK_MAX_COUNTER;
uint16_t HAL_Statis_Shifter = HAL_STATIS_SHIFTER;

void HAL_Reset_SysTickTimer()
{
    HW_Init_SysTimer();
    HAL_Start_SysTickTimer();
}

void HAL_Start_SysTickTimer()
{
    HW_Start_SysTimer();
}

uint8_t HAL_Get_SysTickTimer()
{
	return (HW_Read_SysTimer());
}

void HAL_Wait_1us()
{
    HW_Wait_1us();
}

void HAL_Wait_10us()
{
    HW_Wait_10us();
}

void HAL_Wait_20us()
{
    HW_Wait_10us();
    HW_Wait_10us();
}

void HAL_Wait_50us()
{
    HW_Wait_50us();
}

void HAL_Wait_100us()
{
    HW_Wait_100us();
}

void HAL_Wait_200us()
{
    HW_Wait_100us();
    HW_Wait_100us();
}

void HAL_Wait_500us()
{
    HW_Wait_500us();
}

void HAL_Wait_1ms()
{
    HW_Wait_1ms();
}

void HAL_Wait_ms(uint16_t ms)
{
	for (uint16_t i=0; i<ms; i++)
	{
    	HAL_Wait_1ms();
	}

}
