#include <atmel_start.h>


#include "SCHEDULER/SCHEDULER.h"
#include "HAL/HAL_DigOut.h"
#include "HAL/HAL_PwmOut.h"
#include "HAL/HAL_DigIn.h"
#include "HAL/HAL_Timer.h"


int main(void)
{
	/* Initializes MCU, drivers and middleware */
	atmel_start_init();

    HAL_Init_DigOuts();
    HAL_Init_PwmOuts();
    HAL_Init_DigIns();

    VR_RTOS_Init();

    uint8_t TaskStartupNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_STARTUP);
    VR_RTOS_Init_Task(TaskStartupNo, 0, 0);
    VR_RTOS_Start_Task(TaskStartupNo);

    VR_RTOS_Run(); // endless loop !!!
	
	
	return 0;
}
