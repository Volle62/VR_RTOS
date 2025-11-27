
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"
#include "HAL/HAL_Timer.h"

void do_Task_NULL()
{
	static bool firstCall=true;
	static uint16_t halfPercent=0; // 0.5 percent. Add it before shifting so we get a correct rounded number
	
	if (firstCall)
	{
		firstCall = false;
		halfPercent = 1 << (HAL_Statis_Shifter - 1);
	}
	
	VR_RTOS_TaskDetail_t *TaskDetail; 
	if (VR_RTOS_getPerformStatistic())
	{
		uint8_t i = 0;
		TaskDetail = VR_RTOS_getTaskDetail(i);
		do
		{
			TaskDetail->PercentTime = (TaskDetail->SumTime + halfPercent) >> HAL_Statis_Shifter;
			TaskDetail->SumTime = 0;
			i++;
			TaskDetail = VR_RTOS_getTaskDetail(i);
		} while (TaskDetail->TaskName != NULL);
		
		VR_RTOS_resetPerformStatistic();
	}	
}
