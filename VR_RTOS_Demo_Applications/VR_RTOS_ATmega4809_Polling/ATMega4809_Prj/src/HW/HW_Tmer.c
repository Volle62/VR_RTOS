#include <atmel_start.h>

#include "HW/HW_timer.h"

#include <compiler.h>

void HW_Init_SysTimer(void)
{
	TCA0.SINGLE.CTRLA = (HW_DIVIDER << 1);
}

void HW_Start_SysTimer(void)
{
	TCA0.SINGLE.CNT = 0x0000;
	TCA0.SINGLE.CTRLA |= 1; // Enable the TCA0
}

void HW_Restart_Timer(void)
{
	TCA0.SINGLE.CTRLA &= ~1; // Disable the TCA0
	HW_Start_SysTimer();
}

uint16_t HW_Read_SysTimer(void)
{
	return (TCA0.SINGLE.CNT);
}

void HW_Wait_1us(void)
{
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
}

void HW_Wait_5us(void)
{
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");

    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");

    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");

    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");

    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");
    asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop"); asm ("nop");

}

void HW_Wait_10us(void)
{
    HW_Wait_5us();
    HW_Wait_5us();
}


void HW_Wait_100us(void)
{
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
    HW_Wait_10us();
}

void HW_Wait_500us(void)
{
    uint16_t Count1 = HW_Read_SysTimer();
    uint16_t Count2;
    uint16_t Limit = HW_SYSTEM_TICK_1MS / 2;

    while (1)
    {
        Count2 = HW_Read_SysTimer();
        if ((Count2 - Count1) > Limit)
            break;
    }
}

void HW_Wait_1ms(void)
{
    uint16_t Count1 = HW_Read_SysTimer();
    uint16_t Count2;
    uint16_t Limit = HW_SYSTEM_TICK_1MS;

    while (1)
    {
        Count2 = HW_Read_SysTimer();
        if ((Count2 - Count1) > Limit)
        break;
    }
}

