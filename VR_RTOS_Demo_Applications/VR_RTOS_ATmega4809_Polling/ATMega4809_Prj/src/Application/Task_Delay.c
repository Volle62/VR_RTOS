#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"
#include "HAL/HAL_DigOut.h"
#include "HAL/HAL_Timer.h"

static uint8_t MyTaskNo=INVALID_TASKNO;
static uint8_t  DOut_TP1_No=0;

void do_Task_Delay(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2)
{
    
    switch (Sig)
    {
	  case SIG_INIT :
		  MyTaskNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_DELAY);
          DOut_TP1_No = HAL_Get_DigOut_No(STR_DIGOUT_TP1);
          break;

	  case SIG_START :
		  // send SIG_TIME to MyTask and start the Task
		  VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, 0, 0, NEXT_TICK);
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

	  case SIG_TIME :
	      VR_RTOS_SendMsg(PRIO_MID, MyTaskNo, MyTaskNo, SIG_TIME, 0, 0, NEXT_TICK);
	      HAL_Set_DigOut(DOut_TP1_No);
          HAL_Wait_1ms();
          //HAL_Wait_100us();
          HAL_Reset_DigOut(DOut_TP1_No);

	      //VR_RTOS_SendMsg(PRIO_LOW, MyTaskNo, MyTaskNo, SIG_DELAY, 0, 0, NO_TICK);
          break;

	  case SIG_DELAY :
            HAL_Set_DigOut(DOut_TP1_No);
            HAL_Wait_100us(1);
            HAL_Reset_DigOut(DOut_TP1_No);
            if (Para1 <= 5)
                VR_RTOS_SendMsg(PRIO_LOW, MyTaskNo, MyTaskNo, SIG_DELAY, Para1 + 1, 0, NO_TICK);
          break;
			
	  default :
		  break;
    }
}
