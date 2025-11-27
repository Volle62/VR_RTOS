#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"
#include "HAL/HAL_DigOut.h"
#include "HAL/HAL_PwmOut.h"
#include "HAL/HAL_DigIn.h"
#include "VR_TOOLS/VR_String.h"

static uint8_t  s_MyTaskNo=INVALID_TASKNO;
static uint8_t  s_DigOut_LED_No=0;
static uint8_t  s_PwmOut_LED_No=0;
static uint8_t  s_DigIn_Switch_No=0;
static uint16_t s_Blink_Time=500; //500ms
static uint8_t  s_Pwm_Value=PWM_MAX_VALUE/2;

extern void Send_Trace(uint8_t Task_From, char * TraceString); // Task_Led.c
 
void do_Task_Led(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2)
{
  static char StrTrace[80];
  static char StrBlinkTime[10];
  static char StrPwmValue[10];

  switch (Sig)
  {
        case SIG_INIT :
            s_MyTaskNo    = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_LED);
            s_DigOut_LED_No = HAL_Get_DigOut_No(STR_DIGOUT_LED);
            s_DigIn_Switch_No = HAL_Get_DigIn_No(STR_DIGIN_SWITCH);
            s_PwmOut_LED_No = HAL_Get_PwmOut_No(STR_PWMOUT_LED);
            HAL_Set_PwmOut_Value(s_PwmOut_LED_No, s_Pwm_Value);
            break;

        case SIG_START :
            // send SIG_TIME to MyTask and start the Task
            VR_RTOS_SendMsg(PRIO_MID, s_MyTaskNo, s_MyTaskNo, SIG_LED_TOGGLE, 0, 0, VR_RTOS_CalcMsToTicks(s_Blink_Time));
            break;

        case SIG_TRACE_OFF :
            VR_RTOS_Set_Task_Trace_On(s_MyTaskNo, false);
            break;

        case SIG_TRACE_ON :
            VR_RTOS_Set_Task_Trace_On(s_MyTaskNo, true);
            break;

        case SIG_TRACE_TOGGLE :
            if (VR_RTOS_Is_Task_Trace_On(s_MyTaskNo))
                VR_RTOS_Set_Task_Trace_On(s_MyTaskNo, false);
            else
                VR_RTOS_Set_Task_Trace_On(s_MyTaskNo, true);
            break;

        case SIG_TIME :
            //VR_RTOS_SendMsg(PRIO_MID, s_MyTaskNo, s_MyTaskNo, SIG_TIME, 0, 0, NEXT_TICK);
            //if (HAL_Get_DigIn(s_DigIn_Switch_No))
                //HAL_Set_DigOut(s_DigOut_LED_No);
            //else
                //HAL_Reset_DigOut(DOut_LED_No);
            break;

        case SIG_LED_TOGGLE :
            VR_RTOS_SendMsg(PRIO_MID, s_MyTaskNo, s_MyTaskNo, SIG_LED_TOGGLE, 0, 0, VR_RTOS_CalcMsToTicks(s_Blink_Time));
            HAL_Toggle_DigOut(s_DigOut_LED_No);
            break;
        
        case SIG_PLUS_LEDPROG :
            s_Blink_Time += 100;
            if (s_Blink_Time > 1000)
                s_Blink_Time = 100;
            VR_Itoa((int32_t)s_Blink_Time, StrBlinkTime);

            s_Pwm_Value++;
            if (s_Pwm_Value > PWM_MAX_VALUE)
                s_Pwm_Value = 0;
            HAL_Set_PwmOut_Value(s_PwmOut_LED_No, s_Pwm_Value);
            VR_Itoa((int32_t)s_Pwm_Value, StrPwmValue);

            VR_StrCpy(StrTrace, "Task_Led : Blink_Time = ", StrBlinkTime, "; Pwm_Value = ",StrPwmValue, "\r\n", NULL);
            Send_Trace(s_MyTaskNo, StrTrace);
            break;
        
        case SIG_MINUS_LEDPROG :
            if (s_Blink_Time >= 200)
                s_Blink_Time -= 100;
            else
                s_Blink_Time = 1000;
            VR_Itoa((int32_t)s_Blink_Time, StrBlinkTime);

            if (s_Pwm_Value > 0)
                s_Pwm_Value++;
            else
                s_Pwm_Value = PWM_MAX_VALUE;
            HAL_Set_PwmOut_Value(s_PwmOut_LED_No, s_Pwm_Value);
            VR_Itoa((int32_t)s_Pwm_Value, StrPwmValue);

            VR_StrCpy(StrTrace, "Task_Led : Blink_Time = ", StrBlinkTime, "; Pwm_Value = ",StrPwmValue, "\r\n", NULL);
            Send_Trace(s_MyTaskNo, StrTrace);
            break;
		
        default :
            break;
	}
}

