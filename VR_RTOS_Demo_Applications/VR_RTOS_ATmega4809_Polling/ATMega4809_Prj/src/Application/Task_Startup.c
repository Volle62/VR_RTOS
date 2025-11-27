/*
 * Task_Startup.c
 *
 * Created: 24.10.2022 19:38:10
 *  Author: 49152
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"

//#define for state machine
#define SM_START_SHELL        1
#define SM_START_DELAY        2
#define SM_START_LED          3
#define SM_START_BOARDDRV     4
#define SM_START_ERRMAN       5

static uint8_t MyTaskNo=INVALID_TASKNO;
static uint8_t TaskShellNo=INVALID_TASKNO;
static uint8_t TaskDelayNo=INVALID_TASKNO;
static uint8_t TaskLedNo=INVALID_TASKNO;
static uint8_t TaskBdDrvNo=INVALID_TASKNO;
static uint8_t TaskErrManNo=INVALID_TASKNO;


void do_Task_StartUp(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2)
{
  switch (Sig)
  {
	  case SIG_INIT :
		  MyTaskNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_STARTUP);
		  TaskShellNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_SHELL);
		  TaskDelayNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_DELAY);
		  TaskLedNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_LED);
		  TaskBdDrvNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_BOARDDRV);
          TaskErrManNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_ERRMAN);
		  break;

	  case SIG_START :
		VR_RTOS_Init_Task(TaskShellNo, 0, 0);
		VR_RTOS_Init_Task(TaskDelayNo, 0, 0);
		VR_RTOS_Init_Task(TaskLedNo, 0, 0);
		VR_RTOS_Init_Task(TaskBdDrvNo, 0, 0);
        VR_RTOS_Init_Task(TaskErrManNo, 0, 0);

		// send SIG_TIME to MyTask and start the TaskRS232
		VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, SM_START_SHELL, 0, NEXT_TICK);
		break;

      case SIG_TIME :
		switch (Para1)
		{
			case SM_START_SHELL:
				VR_RTOS_Start_Task(TaskShellNo);
				VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, SM_START_DELAY, 0, NEXT_TICK);
				break;
			case SM_START_DELAY:
				VR_RTOS_Start_Task(TaskDelayNo);
				VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, SM_START_BOARDDRV, 0, NEXT_TICK);
				break;
			case SM_START_BOARDDRV:
			    VR_RTOS_Start_Task(TaskBdDrvNo);
			    VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, SM_START_LED, 0, NEXT_TICK);
			    break;
			case SM_START_LED:
			    VR_RTOS_Start_Task(TaskLedNo);
			    VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, SM_START_ERRMAN, 0, NEXT_TICK);
			    break;
			case SM_START_ERRMAN:
				//VR_RTOS_Start_Task(TaskErrManNo);
				VR_RTOS_Stop_Task(MyTaskNo);
				break;
		}
		break;
			
		default :
			break;
	}	
}
