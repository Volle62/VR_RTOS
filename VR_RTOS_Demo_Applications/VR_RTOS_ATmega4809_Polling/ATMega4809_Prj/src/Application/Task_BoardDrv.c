#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"
#include "HAL/HAL_DigIn.h"
#include "HAL/HAL_DigOut.h"
#include "HAL/HAL_AnaIn.h"
#include "HAL/HAL_PwmOut.h"

#include "Application\WorkingHour.h"

//*************************
// defines ****************
//*************************
#define TIME_WORKING_HOUR_1S  1000 // 1000ms => 1s
#define TIME_BOARD_DRIVER_MS  2  // 2 ms, because OS_TICK is 2ms
#define MAX_USER_SWITCH_COUNT 10 // 10 times * 2 ms = 20 ms
#define DO_BOARD              0
#define DO_WORKING_HOUR       1


//*************************
// global variables *******
//*************************
WorkingTime_t WorkingTime={0, 0, 0};

//*************************
// static variables *******
//*************************
static uint8_t s_MyTaskNo=INVALID_TASKNO;
static uint8_t s_LedTaskNo=INVALID_TASKNO;
static uint8_t s_DigIn_No_UserSwitch=0;
static uint8_t s_MaxPwmOut=0;

static void process_Switch_Inputs(void);
static void process_AnaIns(void);
static void process_PwmOuts(void);
static void inc_VR_RTOS_WorkingTime(void);

void do_Task_BoardDrv(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2)
{
	switch (Sig)
	{
		case SIG_INIT :
			s_MyTaskNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_BOARDDRV);
			s_LedTaskNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_LED);
			s_DigIn_No_UserSwitch = HAL_Get_DigIn_No(STR_DIGIN_SWITCH);
            HAL_Init_DigOuts();
            HAL_Init_PwmOuts();
            HAL_Init_DigIns();
            HAL_Init_AnaIns();
            s_MaxPwmOut = HAL_get_MaxPwmOut();
			break;

		case SIG_START :
			VR_RTOS_SendMsg(PRIO_HIGH, s_MyTaskNo, s_MyTaskNo, SIG_TIME, DO_BOARD,        0, VR_RTOS_CalcMsToTicks(TIME_BOARD_DRIVER_MS));
			VR_RTOS_SendMsg(PRIO_LOW,  s_MyTaskNo, s_MyTaskNo, SIG_TIME, DO_WORKING_HOUR, 0, VR_RTOS_CalcMsToTicks(TIME_WORKING_HOUR_1S));
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
            if (Para1 == DO_BOARD)
            {
			    VR_RTOS_SendMsg(PRIO_HIGH, s_MyTaskNo, s_MyTaskNo, SIG_TIME, DO_BOARD, 0, VR_RTOS_CalcMsToTicks(TIME_BOARD_DRIVER_MS));

			    process_Switch_Inputs();
                process_AnaIns();
                process_PwmOuts();
			}
            else
            {
                VR_RTOS_SendMsg(PRIO_LOW,  s_MyTaskNo, s_MyTaskNo, SIG_TIME, DO_WORKING_HOUR, 0, VR_RTOS_CalcMsToTicks(TIME_WORKING_HOUR_1S));
                inc_VR_RTOS_WorkingTime();
            }
            break;

		default :
			break;
	}
}

static void process_Switch_Inputs()
{
	static uint8_t inpUserSwitchCount=0;
	static bool    inpUserSwitchState=false;
    	
	if (inpUserSwitchState == false)
	{
		if (HAL_Get_DigIn(s_DigIn_No_UserSwitch))
		{
			inpUserSwitchCount++;
			if (inpUserSwitchCount >= MAX_USER_SWITCH_COUNT)
			{
				VR_RTOS_SendMsg(PRIO_MID, s_MyTaskNo, s_LedTaskNo, SIG_PLUS_LEDPROG, 0, 0, NEXT_TICK);
				inpUserSwitchCount = 0;
				inpUserSwitchState = true;
			}
		}
		else
		{
			inpUserSwitchState = 0;
		}
	}
	else
	{
		if (!HAL_Get_DigIn(s_DigIn_No_UserSwitch))
		{
			inpUserSwitchState = 0;
			inpUserSwitchCount = false;
		}
	}
}

static void process_AnaIns(void)
{
    //  // 0 .. 3 according to AnaIn_Detail
    static uint8_t state=0;
    static uint8_t AnaIn_Ch;

    switch (state)
    {
        case 0 :
            AnaIn_Ch = HAL_Get_AnaIn_No(STR_ANAIN_I1);
            HAL_Start_AnaIn(AnaIn_Ch);
            state = 1;
            break;
        case 1 :
            HAL_Read_AnaIn();
            AnaIn_Ch = HAL_Get_AnaIn_No(STR_ANAIN_I1);
            HAL_Start_AnaIn(AnaIn_Ch);
            state = 2;
            break;
        case 2 :
            HAL_Read_AnaIn();
            AnaIn_Ch = HAL_Get_AnaIn_No(STR_ANAIN_I2);
            HAL_Start_AnaIn(AnaIn_Ch);
            state = 3;
            break;
        case 3 :
            HAL_Read_AnaIn();
            AnaIn_Ch = HAL_Get_AnaIn_No(STR_ANAIN_I3);
            HAL_Start_AnaIn(AnaIn_Ch);
            state = 4;
            break;
        case 4 :
            HAL_Read_AnaIn();
            AnaIn_Ch = HAL_Get_AnaIn_No(STR_ANAIN_TEMP);
            HAL_Start_AnaIn(AnaIn_Ch);
            state = 1;
            break;
    }
}

static void process_PwmOuts(void)
{
    // realizing PWM with software because ATMega has to less timers
    // if you have enough timers you can implement by HW timers
    static uint8_t s_counter=0;

    //return;
    
    s_counter++;
    if (s_counter > PWM_MAX_VALUE)
        s_counter = 0;

    for (uint8_t i=0; i<s_MaxPwmOut; i++)
    {
        uint8_t pwmValue = HAL_Get_PwmOut_Value(i);

        if (pwmValue >= PWM_MAX_VALUE)
        {
            HAL_Set_PwmOut_State(i, true);
        }
        else if (pwmValue == 0)
        {
            HAL_Set_PwmOut_State(i, false);
        }
        else 
        {
            HAL_Set_PwmOut_State(i, (s_counter < pwmValue));
        }
    }
}

static void inc_VR_RTOS_WorkingTime()
{
    WorkingTime.Seconds++;

    if (WorkingTime.Seconds > 59)
    {
        WorkingTime.Seconds = 0;
        WorkingTime.Minutes++;
    }

    if (WorkingTime.Minutes > 59)
    {
        WorkingTime.Minutes = 0;
        WorkingTime.Hour++;
    }
}
