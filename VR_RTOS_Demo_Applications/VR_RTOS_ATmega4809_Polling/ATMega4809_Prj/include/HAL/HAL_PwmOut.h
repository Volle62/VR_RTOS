// HAL_PwmOut.h

#ifndef HAL_PWMOUT_H_
#define HAL_PWMOUT_H_

#include "HAL/HAL_Config/HAL_PwmOut_Cfg.h"



void HAL_Init_PwmOuts(void);
void HAL_Set_PwmOut_Value(uint8_t pwmOut_no, uint8_t value);
void HAL_Set_PwmOut_State(uint8_t pwmOut_no, bool state); // only need on SW-PWM
uint8_t HAL_Get_PwmOut_Value(uint8_t pwmOut_no);
bool HAL_Get_PwmOut_State(uint8_t pwmOut_no); // only need on SW-PWM

char *HAL_Get_PwmOut_Name(uint8_t pwmOut_no);
uint8_t HAL_Get_PwmOut_No(char *PwmOut_Name);
uint8_t HAL_get_MaxPwmOut();
uint16_t getNumberOfPwmOuts(void);

typedef struct
{
	char   *PwmOut_Name;
	uint8_t Port; // for example 'B'
	uint8_t Pin;  // for example 5
    bool    Invert;
    uint8_t Value; // 0 .. MaxValue means 0 .. 100 Percent
} HAL_PwmOut_t;


#endif /* HAL_PWMOUT_H_ */