#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"

#define MAX_ERRORLIST    10

static uint8_t MyTaskNo=INVALID_TASKNO;

static VR_RTOS_Error_t ErrorList[MAX_ERRORLIST];

static uint8_t No_Of_Errors=0;

void do_Task_ErrorManager(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2)
{
  switch (Sig)
  {
	  case SIG_INIT :
	      MyTaskNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_ERRMAN);
	      memset(ErrorList, 0, sizeof(ErrorList));
	      break;

	  case SIG_START :
	    break;

	  case SIG_TRACE_OFF :
	      VR_RTOS_Set_Task_Trace_On(MyTaskNo, false);
	      break;

	  case SIG_TRACE_ON :
	      VR_RTOS_Set_Task_Trace_On(MyTaskNo, true);
	      break;

	  case SIG_TRACE_TOGGLE :
	      if (VR_RTOS_Is_Task_Trace_On(MyTaskNo))
	        VR_RTOS_Set_Task_Trace_On(MyTaskNo, false);
	      else
	        VR_RTOS_Set_Task_Trace_On(MyTaskNo, true);
	      break;

	  case SIG_ERR_UNKNOWN :
	      if (No_Of_Errors < MAX_ERRORLIST)
	      {
    	      ErrorList[No_Of_Errors].ErrorSig = SIG_ERR_UNKNOWN;
    	      ErrorList[No_Of_Errors].ErrorNumber = 0;
    	      ErrorList[No_Of_Errors].TaskNo = Para1;
    	      ErrorList[No_Of_Errors].TaskSig = Para2;
    	      ErrorList[No_Of_Errors].TicksSinceStart = VR_RTOS_getTicksSinceStart();
    	      No_Of_Errors++;
	      }
	      break;

	  case SIG_ERR_SYS :
	      if (No_Of_Errors < MAX_ERRORLIST)
	      {
    	      ErrorList[No_Of_Errors].ErrorSig = SIG_ERR_SYS;
    	      ErrorList[No_Of_Errors].ErrorNumber = 0;
    	      ErrorList[No_Of_Errors].TaskNo = Para1;
    	      ErrorList[No_Of_Errors].TaskSig = Para2;
    	      ErrorList[No_Of_Errors].TicksSinceStart = VR_RTOS_getTicksSinceStart();
    	      No_Of_Errors++;
	      }
	      break;

	  case SIG_ERR_LOST_MSG :
	      if (No_Of_Errors < MAX_ERRORLIST)
	      {
    	      ErrorList[No_Of_Errors].ErrorSig = SIG_ERR_LOST_MSG;
    	      ErrorList[No_Of_Errors].ErrorNumber = 0;
    	      ErrorList[No_Of_Errors].TaskNo = Para1;
    	      ErrorList[No_Of_Errors].TaskSig = Para2;
    	      ErrorList[No_Of_Errors].TicksSinceStart = VR_RTOS_getTicksSinceStart();
    	      No_Of_Errors++;
	      }
	      break;

	  case SIG_ERR_OVERTIME :
	      if (No_Of_Errors < MAX_ERRORLIST)
	      {
    	      ErrorList[No_Of_Errors].ErrorSig = SIG_ERR_OVERTIME;
    	      ErrorList[No_Of_Errors].ErrorNumber = 0;
    	      ErrorList[No_Of_Errors].TaskNo = Para1;
    	      ErrorList[No_Of_Errors].TaskSig = Para2;
    	      ErrorList[No_Of_Errors].TicksSinceStart = VR_RTOS_getTicksSinceStart();
    	      No_Of_Errors++;
	      }
	      break;

	  case SIG_ERR_COMMON :
	      if (No_Of_Errors < MAX_ERRORLIST)
	      {
    	      ErrorList[No_Of_Errors].ErrorSig = SIG_ERR_COMMON;
    	      ErrorList[No_Of_Errors].ErrorNumber = 0;
    	      ErrorList[No_Of_Errors].TaskNo = Para1;
    	      ErrorList[No_Of_Errors].TaskSig = Para2;
    	      ErrorList[No_Of_Errors].TicksSinceStart = VR_RTOS_getTicksSinceStart();
    	      No_Of_Errors++;
	      }
	      break;
	  
	  default :
		  break;
  }
}

VR_RTOS_Error_t *getErrorManager_Error(uint8_t No)
{
    if (No < No_Of_Errors)
        return (&ErrorList[No]);
    else
        return (NULL);
}


