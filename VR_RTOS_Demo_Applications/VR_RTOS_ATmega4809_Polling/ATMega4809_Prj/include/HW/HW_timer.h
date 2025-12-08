
#ifndef HW_TIMER_H_INCLUDED
#define HW_TIMER_H_INCLUDED

#include <compiler.h>

#define OS_TICK_1_MS         1  //Tick size 1 ms   : Limit = 19, divider = 0x07 => 1024
#define OS_TICK_1_MS_STRING  "1ms"
#define OS_TICK_2_MS          2  //Tick size 2 ms   : Limit = 39, divider = 0x07 => 1024
#define OS_TICK_2_MS_STRING  "2ms"
#define OS_TICK_4_MS          4  //Tick size 4 ms   : Limit = 78, divider = 0x07 => 1024
#define OS_TICK_4_MS_STRING  "4ms"
#define OS_TICK_5_MS          5  //Tick size 5 ms   : Limit = 98, divider = 0x07 => 1024
#define OS_TICK_5_MS_STRING  "5ms"
#define OS_TICK_10_MS         10  //Tick size 5 ms   : Limit = 98, divider = 0x07 => 1024
#define OS_TICK_10_MS_STRING "10ms"


// Timer is running with 20MHz
//#define HW_DIVIDER        0x05  // Divide by 64
//#define HW_DIVIDER        0x06  // Divide by 256
//#define HW_DIVIDER        0x07  // Divide by 1024

#define HW_OS_TICK_MS         OS_TICK_2_MS
#define HW_OS_TICK_MS_STRING  OS_TICK_2_MS_STRING

#define HW_DIVIDER          0x07  // Divide by 1024
#define HW_SYSTEM_TICK_1MS  19

#if (HW_OS_TICK_MS == OS_TICK_1_MS)
#define HW_OS_TICK_MAX_COUNTER     19
#define HW_STATIS_TICK_MAX_COUNTER 337
#define HW_STATIS_SHIFTER          6
#endif

#if (HW_OS_TICK_MS == OS_TICK_2_MS)
#define HW_OS_TICK_MAX_COUNTER      39
#define HW_STATIS_TICK_MAX_COUNTER  164
#define HW_STATIS_SHIFTER           6
#endif

#if (HW_OS_TICK_MS == OS_TICK_4_MS)
#define HW_OS_TICK_MAX_COUNTER      78
#define HW_STATIS_TICK_MAX_COUNTER  82
#define HW_STATIS_SHIFTER           6
#endif

#if (HW_OS_TICK_MS == OS_TICK_5_MS)
#define HW_OS_TICK_MAX_COUNTER      97
#define HW_STATIS_TICK_MAX_COUNTER  66
#define HW_STATIS_SHIFTER           6
#endif

#if (HW_OS_TICK_MS == OS_TICK_10_MS)
#define HW_OS_TICK_MAX_COUNTER      195
#define HW_STATIS_TICK_MAX_COUNTER  33
#define HW_STATIS_SHIFTER           6
#endif


#ifdef __cplusplus
extern "C" {
#endif

void HW_Init_SysTimer(void);
void HW_Start_SysTimer(void);
void HW_Restart_Timer(void);
uint16_t HW_Read_SysTimer(void);
void HW_Wait_1us(void);
void HW_Wait_10us(void);
void HW_Wait_50us(void);
void HW_Wait_100us(uint8_t Factor); // Factor can be 1 .. 10
void HW_Wait_1ms(void);
#ifdef __cplusplus
}
#endif

#endif /* HW_TIMER_H_INCLUDED */
