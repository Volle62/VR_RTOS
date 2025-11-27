// Volker Rumm, 01.10.2019                                                                         *


#ifndef SCHEDULER_H
#define SCHEDULER_H

#define VR_RTOS_SCHEDULER_VERSION    "VR_RTOS_SCHEDULER POLLING 1.0.1"

typedef unsigned char uint8_t;
typedef signed char   char_t;
typedef unsigned int  uint16_t;
typedef unsigned long uint32_t;

#define TRUE    true
#define FALSE   false

#define TRACE_CSTR(x)  VR_RTOS_Trace_CStr(MY_TASK, x)
#define TRACE_U8(x)    VR_RTOS_Trace_U8(MY_TASK, x)

#define IN_USE            1
#define NOT_IN_USE        0
#define INVALID_TASKNO    0xFF
#define SCHEDULER_TASKNO  0xFE
#define RS232_TASKNO      0xFD

typedef uint16_t  para_t;
typedef uint16_t  tick_t;

#include "SCHEDULER/SCHEDULER_Config/SCHEDULER_CFG.h"

typedef enum {
	PRIO_LOW = 0,
	PRIO_MID,
	PRIO_HIGH,
} VR_RTOS_Prio_e;

typedef struct
{
    uint32_t    TicksSinceStart;
    uint16_t    ErrorSig;
    uint16_t    TaskNo;
    uint16_t    TaskSig;
    uint16_t    ErrorNumber;
} VR_RTOS_Error_t;

typedef struct
{
	char *TaskName;                                     // Name of the task
	void (*doTaskFunc)(uint8_t, Sig_e, para_t, para_t); // the function of the task
	bool     Task_On;                                   // task aktive, always false at start
	bool     Trace_On;                                  // trace active, always false at start
	uint16_t SumTime;                                   // sum of time, always 0 at start; TW-Timer
	uint8_t  PercentTime;                               // sum of time calculated to percent, always 0 at start
	uint8_t  MaxTime;                                   // max count of TW-Timer ticks to detect overtime error, 0 if no detection wanted
} VR_RTOS_TaskDetail_t;

#define NO_TICK     ((tick_t)0)
#define NEXT_TICK   ((tick_t)1)

extern bool VR_RTOS_IsRunning;

// common VR_RTOS Functions
extern void VR_RTOS_SendMsg_Broadcast(VR_RTOS_Prio_e Prio, Sig_e Sig, para_t Para1, para_t Para2);
extern void VR_RTOS_SendMsg(VR_RTOS_Prio_e Prio, uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick);
extern void VR_RTOS_Init();
extern uint8_t VR_RTOS_getMaxTask();
extern tick_t VR_RTOS_CalcMsToTicks(uint16_t ms);
extern void VR_RTOS_Run();
extern void VR_RTOS_resetPerformStatistic();
extern bool VR_RTOS_getPerformStatistic();
extern uint32_t VR_RTOS_getMessagesOverTime();
extern uint32_t VR_RTOS_get_Lost_Messages();
extern void VR_RTOS_Tick();
extern uint32_t VR_RTOS_getTicksSinceStart();
extern void VR_RTOS_Init_Task(uint8_t Task, para_t Para1, para_t Para2);
extern void VR_RTOS_Start_Task(uint8_t Task);
extern void VR_RTOS_Stop_Task(uint8_t Task);
extern bool VR_RTOS_Is_Task_On(uint8_t Task);
extern bool VR_RTOS_Is_Task_Trace_On(uint8_t Task);
extern void VR_RTOS_Set_Task_Trace_On(uint8_t Task, bool TraceOn);

extern void VR_RTOS_set_Tick_TaskNULL(tick_t Tick);
uint32_t VR_RTOS_get_Tick_TaskNULL();

extern char *VR_RTOS_Get_SysTick_String();

extern uint8_t *VR_RTOS_Get_TaskName(uint8_t Task);
extern uint8_t VR_RTOS_Get_TaskNo(uint8_t *TaskName);

extern uint32_t VR_RTOS_get_Lost_Messages();
extern uint32_t VR_RTOS_get_Tick_Collisions();

extern VR_RTOS_TaskDetail_t *VR_RTOS_getTaskDetail(uint8_t index);

extern void VR_RTOS_Trace_U8(uint8_t myTask, uint8_t *Strg);
extern void VR_RTOS_Trace_CStr(uint8_t myTask, char *Strg);

#endif // SCHEDULER_H
