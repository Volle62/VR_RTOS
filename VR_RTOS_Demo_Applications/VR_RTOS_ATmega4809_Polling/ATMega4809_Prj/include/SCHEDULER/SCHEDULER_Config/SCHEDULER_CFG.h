// Volker Rumm, 01.10.2019                                                                         *

#ifndef SCHEDULER_CFG_H
#define SCHEDULER_CFG_H

#define VR_RTOS_HIGH_PRIO_USE        IN_USE
#define VR_RTOS_LOW_PRIO_USE         IN_USE
#define VR_RTOS_NULLTASK_USE         IN_USE

#define VR_RTOS_TICK_COLLISION_COUNT IN_USE

#define VR_RTOS_HISTORY              NOT_IN_USE
#define VR_RTOS_HISTORY_SIZE         100

#define MAX_MSGS_HIGH     10
#define MAX_MSGS_MID      20
#define MAX_MSGS_LOW      20

// Messages for Tasks
// Very important : lowest valid Number for Sig is 1 !!!
typedef enum {
  SIG_EMPTY = 0,
  SIG_INIT = 1,
  SIG_START,
  SIG_STOP,
  SIG_TIME,
  
  // Task_Delay
  SIG_DELAY,
  
  // Task_RS232
  SIG_SEND,
  SIG_RECV,
  
  // Task_Trace
  SIG_TRACE,
  SIG_TRACE_TOGGLE,
  SIG_TRACE_ON,
  SIG_TRACE_OFF,
  
  // Task_Shell
  SIG_RPT_CMD_SYS,
  SIG_RPT_CMD_VER,
  SIG_RPT_CMD_ERR,
  SIG_RPT_CMD_DI,
  SIG_RPT_CMD_DO,
  SIG_RPT_CMD_AI,
  SIG_RPT_CMD_AO,
  SIG_RPT_CMD_PO,
  SIG_RPT_CMD_HELP,
  
  // Task_LED
  SIG_LED_TOGGLE,
  SIG_PLUS_LEDPROG,
  SIG_MINUS_LEDPROG,

  SIG_ERR_LOST_MSG, //=10000,
  SIG_ERR_OVERTIME, //=10001,
  SIG_ERR_UNKNOWN, //=10002,
  SIG_ERR_SYS, //=10003,
  SIG_ERR_COMMON, //=10004,
  
  SIG_LAST
} Sig_e;

extern void do_Task_RS232();
extern void do_Task_AnaIn();

extern void do_Task_StartUp(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2);
extern void do_Task_Shell(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2);
extern void do_Task_Led(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2);
extern void do_Task_BoardDrv(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2);
extern void do_Task_Delay(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2);
extern void do_Task_ErrorManager(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2);

#define STR_TASK_STARTUP  "Task_Startup"
#define STR_TASK_SHELL    "Task_Shell"
#define STR_TASK_LED      "Task_LED"
#define STR_TASK_BOARDDRV "Task_BoardDrv"
#define STR_TASK_DELAY    "Task_Delay"
#define STR_TASK_ERRMAN   "Task_ErrMan"

#endif // SCHEDULER_CFG_H
