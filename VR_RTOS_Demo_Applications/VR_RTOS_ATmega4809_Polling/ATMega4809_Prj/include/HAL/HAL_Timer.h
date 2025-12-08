
#ifndef HAL_TIMER_H_INCLUDED
#define HAL_TIMER_H_INCLUDED

#include <compiler.h>

#include "HW/HW_timer.h"

#define HAL_OS_TICK_MS               HW_OS_TICK_MS
#define HAL_OS_TICK_MS_STRING        HW_OS_TICK_MS_STRING
#define HAL_OS_TICK_MAX_COUNTER      HW_OS_TICK_MAX_COUNTER
#define HAL_STATIS_TICK_MAX_COUNTER  HW_STATIS_TICK_MAX_COUNTER
#define HAL_STATIS_SHIFTER           HW_STATIS_SHIFTER

extern uint16_t HAL_OS_Tick_ms;
extern uint16_t HAL_OS_Tick_MaxCounter;
extern uint16_t HAL_Statis_Tick_MaxCounter;
extern uint16_t HAL_Statis_Shifter;


extern void HAL_Reset_SysTickTimer(void);
extern void HAL_Start_SysTickTimer(void);
extern uint8_t HAL_Get_SysTickTimer(void);

extern void HAL_Wait_1us(void);
extern void HAL_Wait_10us(void);
extern void HAL_Wait_100us(uint8_t Factor);
extern void HAL_Wait_1ms(void);
extern void HAL_Wait_ms(uint16_t ms);

#endif /* HAL_TIMER_H_INCLUDED */
