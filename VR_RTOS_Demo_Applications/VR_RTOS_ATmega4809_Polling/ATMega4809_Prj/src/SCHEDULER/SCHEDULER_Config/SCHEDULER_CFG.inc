#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"
#include "SCHEDULER/SCHEDULER_Config/SCHEDULER_Cfg.h"

VR_RTOS_TaskDetail_t TaskArray[] =  
{
	//TaskName	         doTaskFunc            Task_On    Trace_On  SumTime  PercentTime MaxTime
	{STR_TASK_STARTUP,   do_Task_StartUp,      false,     false,    0,       0,          0},
	{STR_TASK_SHELL,     do_Task_Shell,        false,     false,    0,       0,          0},
	{STR_TASK_LED,       do_Task_Led,          false,     false,    0,       0,          0},
	{STR_TASK_BOARDDRV,  do_Task_BoardDrv,     false,     false,    0,       0,          0},
	{STR_TASK_DELAY,     do_Task_Delay,        false,     false,    0,       0,          0},
	{STR_TASK_ERRMAN,    do_Task_ErrorManager, false,     false,    0,       0,          0},
};


void SchedulerTask()
{
	do_Task_RS232();
    //do_Task_AnaIn();
}
