// Volker Rumm, 01.10.2019                                                                         *

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "HAL/HAL_Timer.h"
#include "SCHEDULER/SCHEDULER.h"

extern void SchedulerTask(void);
static uint16_t getNumberOfTasks(void);


#include "SCHEDULER_Config\SCHEDULER_CFG.inc"


typedef struct
{
    Sig_e   Sig;
    para_t  Para1;
    para_t  Para2;
    tick_t  Tick;
    uint8_t TaskFrom;
    uint8_t TaskSend;
} t_MsgPara;

#if VR_RTOS_LOW_PRIO_USE == IN_USE
t_MsgPara MsgArrLow[MAX_MSGS_LOW];
#endif

t_MsgPara MsgArrMid[MAX_MSGS_MID];

#if VR_RTOS_HIGH_PRIO_USE == IN_USE
t_MsgPara MsgArrHigh[MAX_MSGS_HIGH];
#endif

#if VR_RTOS_NULLTASK_USE == IN_USE
extern void do_Task_NULL();
#endif

#if (VR_RTOS_TICK_COLLISION_COUNT == IN_USE)
static uint32_t TickCollisionCounter = 0;
#endif

bool VR_RTOS_IsRunning=false;

static uint32_t VR_RTOS_TicksSinceStart=0;
static uint32_t VR_RTOS_LostMessages=0;
static uint32_t VR_RTOS_MessagesOverTime=0;
static bool     VR_RTOS_PerformStatistic=false;
static uint8_t  VR_RTOS_Tick_Counter=0;


#if VR_RTOS_NULLTASK_USE == IN_USE
static uint32_t TaskNULL_Tick = 0;
static bool bTask_NULL_On = false;
#endif

extern VR_RTOS_TaskDetail_t TaskArray[];

static bool doTick = false;
static uint8_t MaxTask=0;

static void /*inline*/ VR_RTOS_doTheTick();
static void VR_RTOS_Do_Task_X(t_MsgPara *Msg);
static void Clear_Gap_MsgArr(t_MsgPara *MsgArray, uint8_t Max_Msg, uint8_t index);

#if VR_RTOS_LOW_PRIO_USE == IN_USE
static void VR_RTOS_SendMsg_Broadcast_LowPrio(Sig_e Sig, para_t Para1, para_t Para2);
static void VR_RTOS_SendMsg_LowPrio(uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick);
#endif
static void VR_RTOS_SendMsg_Broadcast_MidPrio(Sig_e Sig, para_t Para1, para_t Para2);
static void VR_RTOS_SendMsg_MidPrio(uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick);
#if VR_RTOS_HIGH_PRIO_USE == IN_USE
static void VR_RTOS_SendMsg_Broadcast_HighPrio(Sig_e Sig, para_t Para1, para_t Para2);
static void VR_RTOS_SendMsg_HighPrio(uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick);
#endif

static uint8_t TaskErrorManNo;

tick_t VR_RTOS_CalcMsToTicks(uint16_t ms)
{
    tick_t ticks = ms / HAL_OS_Tick_ms;
    
    return (ticks);
}

uint8_t VR_RTOS_getMaxTask()
{
    return (MaxTask);
}

char *VR_RTOS_Get_SysTick_String()
{
    return (HAL_OS_TICK_MS_STRING);
}

void VR_RTOS_resetPerformStatistic()
{
    VR_RTOS_PerformStatistic = false;
    VR_RTOS_Tick_Counter = 0;
}

bool VR_RTOS_getPerformStatistic()
{
    return (VR_RTOS_PerformStatistic);
}

uint32_t VR_RTOS_getTicksSinceStart()
{
    return (VR_RTOS_TicksSinceStart);
}

uint32_t VR_RTOS_getMessagesOverTime()
{
    return (VR_RTOS_MessagesOverTime);
}

#if VR_RTOS_NULLTASK_USE == IN_USE
void VR_RTOS_set_Tick_TaskNULL(tick_t Tick)
{
    TaskNULL_Tick = Tick;
}

uint32_t VR_RTOS_get_Tick_TaskNULL()
{
    return (TaskNULL_Tick);
}
#endif

static void /*inline*/ VR_RTOS_doTheTick()
{
    uint8_t i;
    
    #ifdef USE_HW_WATCHDOG
        hw_clr_WDog();
    #endif

    VR_RTOS_TicksSinceStart++;
            
    #if VR_RTOS_LOW_PRIO_USE == IN_USE
        for (i=0; i<MAX_MSGS_LOW; i++)
            if (MsgArrLow[i].Tick > 0)
                MsgArrLow[i].Tick--;
    #endif

    for (i=0; i<MAX_MSGS_MID; i++)
        if (MsgArrMid[i].Tick > 0)
            MsgArrMid[i].Tick--;

    #if VR_RTOS_HIGH_PRIO_USE == IN_USE
        for (i=0; i<MAX_MSGS_HIGH; i++)
            if (MsgArrHigh[i].Tick > 0)
                MsgArrHigh[i].Tick--;
    #endif

    #if VR_RTOS_NULLTASK_USE == IN_USE
        if (TaskNULL_Tick > 0)
            TaskNULL_Tick--;

        bTask_NULL_On = true;
    #endif
}
static void Clear_Gap_MsgArr(t_MsgPara *MsgArray, uint8_t Max_Msg, uint8_t index)
{
    if (MsgArray[index+1].Sig != SIG_EMPTY) // if the following message has a content
    {
        // then fill the gap
        for (; index < Max_Msg-1; index++)
        {
            memcpy(&MsgArray[index], &MsgArray[index+1], sizeof(t_MsgPara));
        }
        memset(&MsgArray[index], 0, sizeof(t_MsgPara));
    }
}

#if VR_RTOS_LOW_PRIO_USE == IN_USE
bool VR_RTOS_Do_Msg_LowPrio()
{
    for (uint8_t index = 0; index < MAX_MSGS_LOW; index++)
    {
        t_MsgPara *Msg = &MsgArrLow[index];
        if ((Msg->Tick == 0) && Msg->Sig)
        {
            VR_RTOS_Do_Task_X(Msg);
            Clear_Gap_MsgArr(MsgArrLow, MAX_MSGS_LOW, index);
            return (true);
        }
    }

    return (false);
}
#endif

bool /*inline*/ VR_RTOS_Do_Msg_MidPrio()
{
    for (uint8_t index = 0; index < MAX_MSGS_MID; index++)
    {
        t_MsgPara *Msg = &MsgArrMid[index];
        if ((Msg->Tick == 0) && Msg->Sig)
        {
            VR_RTOS_Do_Task_X(Msg);
            Clear_Gap_MsgArr(MsgArrMid, MAX_MSGS_MID, index);
            return (true);
        }
    }
    return (false);
}

#if VR_RTOS_HIGH_PRIO_USE == IN_USE
bool /*inline*/ VR_RTOS_Do_Msg_HighPrio()
{
    for (uint8_t index = 0; index < MAX_MSGS_HIGH; index++)
    {
        t_MsgPara *Msg = &MsgArrHigh[index];
        if ((Msg->Tick == 0) && Msg->Sig)
        {
            VR_RTOS_Do_Task_X(Msg);
            Clear_Gap_MsgArr(MsgArrHigh, MAX_MSGS_HIGH, index);
            return (true);
        }
    }
    return (false);
}
#endif

static void VR_RTOS_Do_Task_X(t_MsgPara *Msg)
{
    uint8_t Task;
    uint16_t Cnt_Begin, Cnt_End, Cnt_Diff;

    Task = Msg->TaskSend;
    if (TaskArray[Task].Task_On)
    {
        Cnt_Begin = HAL_Get_SysTickTimer();
        
        TaskArray[Task].doTaskFunc(Msg->TaskFrom, Msg->Sig, Msg->Para1, Msg->Para2);

        Cnt_End = HAL_Get_SysTickTimer();
        Cnt_Diff = Cnt_End - Cnt_Begin;
        if (!VR_RTOS_PerformStatistic)
        {
            TaskArray[Task].SumTime = TaskArray[Task].SumTime + Cnt_Diff;
        }
        
        if (TaskArray[Task].MaxTime > 0)
        {
            if ((TaskArray[Task].MaxTime > 0) && (Cnt_Diff > TaskArray[Task].MaxTime))
            {
                VR_RTOS_SendMsg(PRIO_HIGH, SCHEDULER_TASKNO, TaskErrorManNo, SIG_ERR_OVERTIME, Task, Msg->Sig, NO_TICK);
                VR_RTOS_MessagesOverTime++;
            }
        }

        #if (VR_RTOS_HISTORY == IN_USE)
            MsgHistory[MsgHistoryIdx].Task = Task;
            MsgHistory[MsgHistoryIdx].Sig = Msg->Sig;
            MsgHistory[MsgHistoryIdx].Para1 = Msg->Para1;
            MsgHistory[MsgHistoryIdx].Para2 = Msg->Para2;
            MsgHistory[MsgHistoryIdx].TickSinceStart = VR_RTOS_TicksSinceStart;
            MsgHistory[MsgHistoryIdx].Duration_us = (Cnt_Begin - Cnt_End) * 20;
            MsgHistoryIdx++;
            if (MsgHistoryIdx >= VR_RTOS_HISTORY_SIZE)
                MsgHistoryIdx = 0;
        #endif
    }
    memset(Msg, 0, sizeof(t_MsgPara));
}

static uint16_t getNumberOfTasks(void)
{
    uint16_t NumberOfTasks;

    NumberOfTasks = sizeof(TaskArray) / sizeof(TaskArray[0]);

    return (NumberOfTasks);
}

void VR_RTOS_Init()
{
    uint8_t i;

    MaxTask = getNumberOfTasks();

    #if VR_RTOS_LOW_PRIO_USE == IN_USE
        for (i=0; i<MAX_MSGS_LOW; i++)
            MsgArrLow[i].Sig = SIG_EMPTY;
    #endif

    for (i=0; i<MAX_MSGS_MID; i++)
        MsgArrMid[i].Sig = SIG_EMPTY;

    #if VR_RTOS_HIGH_PRIO_USE == IN_USE
        for (i=0; i<MAX_MSGS_HIGH; i++)
            MsgArrHigh[i].Sig = SIG_EMPTY;
    #endif

    TaskErrorManNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_ERRMAN);
}

void VR_RTOS_Run()
{
    VR_RTOS_IsRunning = true;
    HAL_Reset_SysTickTimer();
    
    while (1)
    {
        SchedulerTask();
        
        // Has the timer reached the OS_Tick ?
        if (HAL_Get_SysTickTimer() >= HAL_OS_Tick_MaxCounter)
        {
            HAL_Reset_SysTickTimer();
            doTick = true;
        }

        if (doTick)
        {
            VR_RTOS_Tick_Counter++;
            if (VR_RTOS_Tick_Counter > HAL_Statis_Tick_MaxCounter)
            {
                VR_RTOS_PerformStatistic = true;
            }
            VR_RTOS_doTheTick();
            doTick = false;
        }

        #if VR_RTOS_HIGH_PRIO_USE == IN_USE
            // When 1 Msg found then do it and start with loop again
            if (VR_RTOS_Do_Msg_HighPrio())
                continue;
        #endif

        // When 1 Msg found then do it and start with loop again
        if (VR_RTOS_Do_Msg_MidPrio())
            continue;

        #if VR_RTOS_LOW_PRIO_USE == IN_USE
            // When 1 Msg found then do it and start with loop again
            if (VR_RTOS_Do_Msg_LowPrio())
                continue;
        #endif

        #if VR_RTOS_NULLTASK_USE == IN_USE
        // When no Msg found then do NULL task one time per tick
        if (bTask_NULL_On)
        {
            do_Task_NULL();
            bTask_NULL_On = false;
        }
        #endif
    } // end of "while (1)"
}


#if VR_RTOS_LOW_PRIO_USE == IN_USE
static void VR_RTOS_SendMsg_Broadcast_LowPrio(Sig_e Sig, para_t Para1, para_t Para2)
{
    uint8_t task;
    
    for (task=0; task < MaxTask; task++)
        VR_RTOS_SendMsg_LowPrio(0, task, Sig, Para1, Para2, 0);
}
#endif

static void VR_RTOS_SendMsg_Broadcast_MidPrio(Sig_e Sig, para_t Para1, para_t Para2)
{
    uint8_t task;
    
    for (task=0; task < MaxTask; task++)
        VR_RTOS_SendMsg_MidPrio(0, task, Sig, Para1, Para2, 0);
}

#if VR_RTOS_HIGH_PRIO_USE == IN_USE
static void VR_RTOS_SendMsg_Broadcast_HighPrio(Sig_e Sig, para_t Para1, para_t Para2)
{
    uint8_t task;
    
    for (task=0; task < MaxTask; task++)
        VR_RTOS_SendMsg_HighPrio(0, task, Sig, Para1, Para2, 0);
}
#endif

#if VR_RTOS_LOW_PRIO_USE == IN_USE
static void VR_RTOS_SendMsg_LowPrio(uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick)
{
    static uint8_t oldMsgIndex=0;
    uint8_t index = oldMsgIndex;

    if (TaskSend >= MaxTask)
    {
        return;
    }
    else
    {
        if (!TaskArray[TaskSend].Task_On)
        return;
    }

    while (1)
    {
        if (MsgArrLow[index].Sig == SIG_EMPTY)
        {
            MsgArrLow[index].Sig      = Sig;
            MsgArrLow[index].Para1    = Para1;
            MsgArrLow[index].Para2    = Para2;
            MsgArrLow[index].Tick     = Tick;
            MsgArrLow[index].TaskFrom = TaskFrom;
            MsgArrLow[index].TaskSend = TaskSend;
            oldMsgIndex = index;
            return;
        }

        index++;

        if (index == MAX_MSGS_LOW)
        index = 0;

        // if no free entry found then increment LostMessages and return;
        if (index == oldMsgIndex)
        {
            VR_RTOS_SendMsg(PRIO_HIGH, SCHEDULER_TASKNO, TaskErrorManNo, SIG_ERR_LOST_MSG, TaskSend, Sig, NO_TICK);
            VR_RTOS_LostMessages++;
            return;
        }
    }
}
#endif

static void VR_RTOS_SendMsg_MidPrio(uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick)
{
    static uint8_t oldMsgIndex=0;
    uint8_t index = oldMsgIndex;
    
    if (TaskSend >= MaxTask)
    {
        return;
    }
    else
    {
        if (!TaskArray[TaskSend].Task_On)
        return;
    }

    while (1)
    {
        if (MsgArrMid[index].Sig == SIG_EMPTY)
        {
            MsgArrMid[index].Sig      = Sig;
            MsgArrMid[index].Para1    = Para1;
            MsgArrMid[index].Para2    = Para2;
            MsgArrMid[index].Tick     = Tick;
            MsgArrMid[index].TaskFrom = TaskFrom;
            MsgArrMid[index].TaskSend = TaskSend;
            oldMsgIndex = index;
            return;
        }

        index++;

        if (index == MAX_MSGS_MID)
        index = 0;

        // if no free entry found then increment LostMessages and return;
        if (index == oldMsgIndex)
        {
            VR_RTOS_SendMsg(PRIO_HIGH, SCHEDULER_TASKNO, TaskErrorManNo, SIG_ERR_LOST_MSG, TaskSend, Sig, NO_TICK);
            VR_RTOS_LostMessages++;
            return;
        }
    }
}

#if VR_RTOS_HIGH_PRIO_USE == IN_USE
static void VR_RTOS_SendMsg_HighPrio(uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick)
{
    static uint8_t oldMsgIndex=0;
    uint8_t index = oldMsgIndex;

    if (TaskSend >= MaxTask)
    {
        return;
    }
    else
    {
        if (!TaskArray[TaskSend].Task_On)
        return;
    }

    while (1)
    {
        if (MsgArrHigh[index].Sig == SIG_EMPTY)
        {
            MsgArrHigh[index].Sig      = Sig;
            MsgArrHigh[index].Para1    = Para1;
            MsgArrHigh[index].Para2    = Para2;
            MsgArrHigh[index].Tick     = Tick;
            MsgArrHigh[index].TaskFrom = TaskFrom;
            MsgArrHigh[index].TaskSend = TaskSend;
            oldMsgIndex = index;
            return;
        }

        index++;

        if (index == MAX_MSGS_HIGH)
        index = 0;

        // if no free entry found then increment LostMessages and return;
        if (index == oldMsgIndex)
        {
            VR_RTOS_SendMsg(PRIO_HIGH, SCHEDULER_TASKNO, TaskErrorManNo, SIG_ERR_LOST_MSG, TaskSend, Sig, NO_TICK);
            VR_RTOS_LostMessages++;
            return;
        }
    }
}
#endif

void VR_RTOS_Init_Task(uint8_t Task, para_t Para1, para_t Para2)
{
    TaskArray[Task].doTaskFunc(0, SIG_INIT, Para1, Para2);
}

void VR_RTOS_Start_Task(uint8_t Task)
{
    TaskArray[Task].Task_On = true;
    TaskArray[Task].doTaskFunc(0, SIG_START, 0, 0);
}

void VR_RTOS_Stop_Task(uint8_t Task)
{
    TaskArray[Task].Task_On = false;
    TaskArray[Task].doTaskFunc(0, SIG_STOP, 0, 0);
}

bool VR_RTOS_Is_Task_On(uint8_t Task)
{
    return (TaskArray[Task].Task_On);
}

bool VR_RTOS_Is_Task_Trace_On(uint8_t Task)
{
    return (TaskArray[Task].Trace_On);
}

void VR_RTOS_Set_Task_Trace_On(uint8_t Task, bool TraceOn)
{
    TaskArray[Task].Trace_On = TraceOn;
}

uint8_t *VR_RTOS_Get_TaskName(uint8_t Task)
{
    return ((uint8_t *)TaskArray[Task].TaskName);
}

uint8_t VR_RTOS_Get_TaskNo(uint8_t *TaskName)
{
    for (int i=0; i<MaxTask; i++)
    {
        // if Task is not On then return
        if (strcmp((const char*)TaskArray[i].TaskName, (const char*)TaskName) == 0)
            return (i);
    }
    return (INVALID_TASKNO);
}

uint32_t VR_RTOS_get_Lost_Messages()
{
    return (VR_RTOS_LostMessages);
}

uint32_t VR_RTOS_get_Tick_Collisions()
{
    #if (VR_RTOS_TICK_COLLISION_COUNT == IN_USE)
        return (TickCollisionCounter);
    #else
        return (0);
    #endif
}

extern VR_RTOS_TaskDetail_t *VR_RTOS_getTaskDetail(uint8_t index)
{
    return (&TaskArray[index]);
}

void VR_RTOS_SendMsg_Broadcast(VR_RTOS_Prio_e Prio, Sig_e Sig, para_t Para1, para_t Para2)
{
    switch (Prio)
    {
        case PRIO_LOW:
            #if VR_RTOS_LOW_PRIO_USE == IN_USE
                VR_RTOS_SendMsg_Broadcast_LowPrio(Sig, Para1, Para2);
            #endif
            break;
        
        case PRIO_MID:
            VR_RTOS_SendMsg_Broadcast_MidPrio(Sig, Para1, Para2);
            break;
        
        case PRIO_HIGH:
            #if VR_RTOS_HIGH_PRIO_USE == IN_USE
                VR_RTOS_SendMsg_Broadcast_HighPrio(Sig, Para1, Para2);
            #endif
            break;
    }
}

void VR_RTOS_SendMsg(VR_RTOS_Prio_e Prio, uint8_t TaskFrom, uint8_t TaskSend, Sig_e Sig, para_t Para1, para_t Para2, tick_t Tick)
{
    switch (Prio)
    {
        case PRIO_LOW:
            #if VR_RTOS_LOW_PRIO_USE == IN_USE
                VR_RTOS_SendMsg_LowPrio(TaskFrom, TaskSend, Sig, Para1, Para2, Tick);
            #endif
            break;
        
        case PRIO_MID:
            VR_RTOS_SendMsg_MidPrio(TaskFrom, TaskSend, Sig, Para1, Para2, Tick);
            break;
        
        case PRIO_HIGH:
            #if VR_RTOS_HIGH_PRIO_USE == IN_USE
                VR_RTOS_SendMsg_HighPrio(TaskFrom, TaskSend, Sig, Para1, Para2, Tick);
            #endif
            break;
    }
}
//void VR_RTOS_Trace_U8(uint8_t myTask, uint8_t *Strg)
//{
//  VR_RTOS_SendMsg_LowPrio(myTask, TASK_TRACE, SIG_TRACE, (para_t)Strg, (para_t)0, NO_TICK);
//}

//void VR_RTOS_Trace_CStr(uint8_t myTask, char *Strg)
//{
//  VR_RTOS_SendMsg_LowPrio(myTask, TASK_TRACE, SIG_TRACE, (para_t)Strg, (para_t)0, NO_TICK);
//}