/**********************************************************************\
*
* Datei: Task_Shell.c
*
* Projekt : Demoprogramm VR_RTOS
*
* Kurzbeschreibung: siehe Beschreibung der Shell
*                   Daten (Befehle) werden mit SIG_RECV empfangen,
*                   zu sendende Daten werden in RS232_Send_FiFo gespeichert
*
* Datum: 01.02.2025       Autor: Volker Rumm
*
\**********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>

#include "Application/Shell_Version.h"
#include "Application/Application.h"
#include "Application/WorkingHour.h"
#include "SCHEDULER/SCHEDULER.h"
#include "HAL/HAL_Version.h"
#include "VR_TOOLS/VR_FiFo.h"
#include "VR_TOOLS/VR_String.h"
#include "HAL/HAL_DigOut.h"
#include "HAL/HAL_PwmOut.h"
#include "HAL/HAL_DigIn.h"
//#include "HAL/HAL_AnaOut.h" ATMega4809 has no analog outputs
#include "HAL/HAL_AnaIn.h"

#define ESC                27
#define SHELL_PRIO         PRIO_LOW
#define SEND_BUFFER_SIZE   128

typedef enum { eRetComplete, eRetSys, eRetVer, eRetErr, eRetHelp, eRetDI, eRetDO, eRetAI, eRetAO, eRetPO} eRetMode_t;
typedef enum { eModeBase, eModeExpert, eModeFactory, eModeManual, eModeTest, eModeEMV} eMode_t;

typedef struct
{
    char_t     *pTxtCommand;         // the command
    uint8_t    lenCommand;          // len of the command string
    eRetMode_t (*pFunc)(uint8_t*, uint8_t *);  // function pointer
    char_t     *pTxtFlag;            // A=All, B=Base, E=Expert, F=Factory, M=Manual, T=Test, V=EMV
    char_t     *pTxtHelp;            // short Help
} ShellCmd_t;


const char *NameShell=SHELL_VERSION;
const char ModeChar[] = "BEFMTV"; // IMPORTANT : must correspond to typedef eMode_t !!!
const uint8_t sPrompt[] = ">> ";
const uint8_t sExpertPrompt[] = "Expert ";
const uint8_t sFactoryPrompt[] = "Factory ";
const uint8_t sManualPrompt[] = "Manual ";
const uint8_t sTestPrompt[] = "Test ";
const uint8_t sEMVVPrompt[] = "EMV ";

extern VR_FiFo_t RS232_Send_FiFo; // Task_RS232.c

char_t *getPrompt(void); // global function

static void stopRepeat(void);
static eRetMode_t CmdVer(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdRst( uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdWDog(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdStart(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdStop(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdTrace(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdSys(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdErr(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdHelp(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdExptOn(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdExptOff(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdFactOn(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdFactOff(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdManuOn(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdManuOff(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdTestOn(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdTestOff(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDO(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDOs(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDOr(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDOt(uint8_t *txtPara, uint8_t *sndBuffer);
#ifdef HAL_PWMOUT_H_
static eRetMode_t CmdPO(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdPOs(uint8_t *txtPara, uint8_t *sndBuffer);
#endif // #ifdef HAL_PWMOUT_H_
static eRetMode_t CmdDI(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDI(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDIs(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDIr(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdDIt(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdAI(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdAIs(uint8_t *txtPara, uint8_t *sndBuffer);
#ifdef HAL_ANAOUT_H_
static eRetMode_t CmdAO(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdAOs(uint8_t *txtPara, uint8_t *sndBuffer);
#endif // #ifdef HAL_ANAOUT_H_

static eRetMode_t CmdRepDO(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdRepDI(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdRepAI(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdRepAll(uint8_t *txtPara, uint8_t *sndBuffer);
static eRetMode_t CmdX(uint8_t *txtPara, uint8_t *sndBuffer);
#ifdef HAL_PWMOUT_H_
static eRetMode_t CmdRepPO(uint8_t *txtPara, uint8_t *sndBuffer);
#endif // #ifdef HAL_PWMOUT_H_
#ifdef HAL_ANAOUT_H_
static eRetMode_t CmdRepAO(uint8_t *txtPara, uint8_t *sndBuffer);
#endif // #ifdef HAL_ANAOUT_H_

static eRetMode_t RepeatCmdSys(uint8_t *sndBuffer, uint8_t idx);
static eRetMode_t RepeatCmdVer(uint8_t *sndBuffer, uint8_t idx);
static eRetMode_t RepeatCmdErr(uint8_t *sndBuffer, uint8_t idx);
static eRetMode_t RepeatCmdDI(uint8_t *sndBuffer, uint8_t index);
static eRetMode_t RepeatCmdAI(uint8_t *sndBuffer, uint8_t index);
static eRetMode_t RepeatCmdDO(uint8_t *sndBuffer, uint8_t index);
#ifdef HAL_PWMOUT_H_
static eRetMode_t RepeatCmdPO(uint8_t *sndBuffer, uint8_t index);
#endif // #ifdef HAL_PWMOUT_H_
#ifdef HAL_ANAOUT_H_
static eRetMode_t RepeatCmdAO(uint8_t *sndBuffer, uint8_t index);
#endif // #ifdef HAL_ANAOUT_H_
static eRetMode_t RepeatCmdHelp(uint8_t *sndBuffer, uint8_t index);

static void doRepDO(uint8_t *sndBuffer);
static void doRepDI(uint8_t *sndBuffer);
static void doRepAI(uint8_t *sndBuffer);
#ifdef HAL_PWMOUT_H_
static void doRepPO(uint8_t *sndBuffer);
#endif // #ifdef HAL_PWMOUT_H_
#ifdef HAL_ANAOUT_H_
static void doRepAO(uint8_t *sndBuffer);
#endif // #ifdef HAL_ANAOUT_H_

static eRetMode_t interpretRcvBuffer(uint8_t *pRcvBuf, uint8_t *sndBuffer);

static eMode_t actualMode = eModeBase;
static uint8_t repDoMax=0, repDiMax=0, repAiMax=0;
static uint8_t repDoCnt=0, repDiCnt=0, repAiCnt=0;
#ifdef HAL_ANAOUT_H_
    static uint8_t repAoMax=0, repAoCnt=0;
#endif // #ifdef HAL_ANAOUT_H_
#ifdef HAL_PWMOUT_H_
    static uint8_t repPoMax=0, repPoCnt=0;
#endif // #ifdef HAL_PWMOUT_H_
static uint8_t sendBuffer[SEND_BUFFER_SIZE];
static uint16_t SumPercent=0;

static uint8_t MyTaskNo=INVALID_TASKNO;
static uint8_t TaskNo_To_Send=RS232_TASKNO;


const ShellCmd_t CmdTab[] =
{
    {(char_t*)"help",    4, CmdHelp,    (char_t*)"A",   (char_t*)"Help Information"},
    {(char_t*)"ver",     3, CmdVer,     (char_t*)"A",   (char_t*)"Version Information"},
    {(char_t*)"trace",   5, CmdTrace,   (char_t*)"EMT", (char_t*)"Trace on/off"},
    {(char_t*)"sys",     3, CmdSys,     (char_t*)"A",   (char_t*)"system info"},
    {(char_t*)"err",     3, CmdErr,     (char_t*)"A",   (char_t*)"errors"},
    {(char_t*)"rst",     3, CmdRst,     (char_t*)"EMT", (char_t*)"MCU reset"},
    {(char_t*)"wdog",    4, CmdWDog,    (char_t*)"EMT", (char_t*)"Watchdog on"},
    {(char_t*)"start",   5, CmdStart,   (char_t*)"EMT", (char_t*)"start task"},
    {(char_t*)"stop",    4, CmdStop,    (char_t*)"EMT", (char_t*)"stop task"},
    {(char_t*)"expton",  6, CmdExptOn,  (char_t*)"B",   (char_t*)"Expert mode on"},
    {(char_t*)"exptoff", 7, CmdExptOff, (char_t*)"E",   (char_t*)"Expert mode off"},
    {(char_t*)"facton",  6, CmdFactOn,  (char_t*)"B",   (char_t*)"Factory mode on"},
    {(char_t*)"factoff", 7, CmdFactOff, (char_t*)"F",   (char_t*)"Factory mode off"},
    {(char_t*)"manuon",  6, CmdManuOn,  (char_t*)"B",   (char_t*)"Manual mode on"},
    {(char_t*)"manuoff", 7, CmdManuOff, (char_t*)"M",   (char_t*)"Manual mode off"},
    {(char_t*)"teston",  6, CmdTestOn,  (char_t*)"B",   (char_t*)"Test mode on"},
    {(char_t*)"testoff", 7, CmdTestOff, (char_t*)"T",   (char_t*)"Test mode off"},
    {(char_t*)"do",      2, CmdDO,      (char_t*)"EMT", (char_t*)"List all dig outputs"},
    {(char_t*)"dos",     3, CmdDOs,     (char_t*)"EM",  (char_t*)"set a dig output"},
    {(char_t*)"dor",     3, CmdDOr,     (char_t*)"EM",  (char_t*)"reset a dig output"},
    {(char_t*)"dot",     3, CmdDOt,     (char_t*)"EM",  (char_t*)"toggle a dig output"},
    {(char_t*)"di",      2, CmdDI,      (char_t*)"EMT", (char_t*)"List all dig inputs"},
    {(char_t*)"dis",     3, CmdDIs,     (char_t*)"T",   (char_t*)"set a dig input"},
    {(char_t*)"dir",     3, CmdDIr,     (char_t*)"T",   (char_t*)"reset a dig input"},
    {(char_t*)"dit",     3, CmdDIt,     (char_t*)"T",   (char_t*)"toggle a dig input"},
    {(char_t*)"ai",      2, CmdAI,      (char_t*)"EMT", (char_t*)"List all ana inputs"},
    {(char_t*)"ais",     3, CmdAIs,     (char_t*)"T",   (char_t*)"set a ana input"},
#ifdef HAL_PWMOUT_H_
    {(char_t*)"po",      2, CmdPO,      (char_t*)"EMT", (char_t*)"List all pwm outputs"},
    {(char_t*)"pos",     3, CmdPOs,     (char_t*)"EM",  (char_t*)"set a pwm output"},
#endif // #ifdef HAL_PWMOUT_H_
#ifdef HAL_ANAOUT_H_
    {(char_t*)"ao",      2, CmdAO,      (char_t*)"EMT", (char_t*)"List all ana outputs"},
    {(char_t*)"aos",     3, CmdAOs,     (char_t*)"EM",  (char_t*)"set an ana output"},
#endif // #ifdef HAL_ANAOUT_H_
    {(char_t*)"repdo",   5, CmdRepDO,   (char_t*)"EMT", (char_t*)"Repeat list all dig outputs"},
    {(char_t*)"repdi",   5, CmdRepDI,   (char_t*)"EMT", (char_t*)"Repeat list all dig inputs"},
    {(char_t*)"repai",   5, CmdRepAI,   (char_t*)"EMT", (char_t*)"Repeat list all ana inputs"},
#ifdef HAL_PWMOUT_H_
    {(char_t*)"reppo",   5, CmdRepPO,   (char_t*)"EMT", (char_t*)"Repeat list all pwm outputs"},
#endif // #ifdef HAL_PWMOUT_H_
#ifdef HAL_ANAOUT_H_
    {(char_t*)"repao",   5, CmdRepAO,   (char_t*)"EMT", (char_t*)"Repeat list all ana outputs"},
#endif // #ifdef HAL_ANAOUT_H_
    {(char_t*)"repall",  6, CmdRepAll,  (char_t*)"EMT", (char_t*)"Repeat list all ins and outs"},
    {(char_t*)"x",       1, CmdX,       (char_t*)"EMT", (char_t*)"stop the Repeat function"},
    {NULL,               0, NULL,       NULL,           NULL},
};


// special functions for HAL in manu mode
extern void Set_DigOut_Manu(uint8_t digOut_no);
extern void Reset_DigOut_Manu(uint8_t digOut_no);
extern void Toggle_DigOut_Manu(uint8_t digOut_no);
#ifdef HAL_ANAOUT_H_
extern void Set_AnaOut_Manu(uint8_t anaOut_no, uint16_t anaValue);
#endif // #ifdef HAL_ANAOUT_H_
#ifdef HAL_PWMOUT_H_
extern void Set_PwmOut_Manu(uint8_t pwmOut_no, uint8_t pwmValue);
#endif // #ifdef HAL_PWMOUT_H_
// end of special functions for HAL in manu mode

// special functions for HAL in test mode
extern void Set_DigIn_Test(uint8_t digIn_no, bool digVal);
extern void Set_AnaIn_Test(uint8_t AnaIn_no, uint16_t AnaVal);
// end of special functions for HAL in test mode

static void PutFiFoSend(uint8_t * buffer, uint8_t size);
static bool Is_FiFoSend_Empty(void);
static char* getStrTicksSinceStart(uint32_t ticks);
static char* getStrWorkingTime(WorkingTime_t WorkingTime);

extern VR_RTOS_Error_t *getErrorManager_Error(uint8_t No); // Task_ErrorManager

void do_Task_Shell(uint8_t TaskFrom, Sig_e Sig, para_t Para1, para_t Para2)
{
    static uint8_t *pRcvBuf=NULL;
    eRetMode_t ReturnMode;

    switch (Sig)
    {
        case SIG_INIT :
            MyTaskNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_SHELL);
            break;

        case SIG_START :
            PutFiFoSend((uint8_t *)NameShell, strlen((char *)NameShell));
            PutFiFoSend((uint8_t *)"\r\n", 2);
            PutFiFoSend((uint8_t *)sPrompt, strlen((char *)sPrompt));
            break;

        case SIG_TIME :
            VR_RTOS_SendMsg(PRIO_LOW, MyTaskNo, MyTaskNo, SIG_TIME, 0, 0, VR_RTOS_CalcMsToTicks(100));
            sendBuffer[0] = 0;
            if (repDoMax)
            {
                repDoCnt++;
                if (repDoCnt == repDoMax)
                {
                    doRepDO(sendBuffer);
                    repDoCnt = 0;
                }
            }
            if (repDiMax)
            {
                repDiCnt++;
                if (repDiCnt == repDiMax)
                {
                    doRepDI(sendBuffer);
                    repDiCnt = 0;
                }
            }
            if (repAiMax)
            {
                repAiCnt++;
                if (repAiCnt == repAiMax)
                {
                    doRepAI(sendBuffer);
                    repDiCnt = 0;
                }
            }

#ifdef HAL_ANAOUT_H_
            if (repAoMax)
            {
                repAoCnt++;
                if (repAoCnt == repAoMax)
                {
                    doRepAO(sendBuffer);
                    repAoCnt = 0;
                }
            }
#endif // #ifdef HAL_ANAOUT_H_

#ifdef HAL_PWMOUT_H_
            if (repPoMax)
            {
                repPoCnt++;
                if (repPoCnt == repPoMax)
                {
                    doRepPO(sendBuffer);
                    repPoCnt = 0;
                }
            }
#endif // #ifdef HAL_PWMOUT_H_

            if (sendBuffer[0])
            {
                VR_StrCat((char *)sendBuffer, getPrompt(), NULL);
                VR_RTOS_SendMsg(PRIO_LOW, MyTaskNo, MyTaskNo, SIG_SEND, (para_t)sendBuffer, (para_t)strlen((char*)sendBuffer), NEXT_TICK);
            }
            break;

        case SIG_RECV :
            TaskNo_To_Send = TaskFrom;
            if (Para1 == ESC)
            {
                stopRepeat();
            }
            else
            {
                pRcvBuf = (uint8_t *)Para1;
                ReturnMode = interpretRcvBuffer(pRcvBuf, sendBuffer);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    VR_StrCat((char *)sendBuffer, (uint8_t *)getPrompt(), NULL);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                }
                else if (ReturnMode == eRetSys)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_SYS, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetVer)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_VER, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetErr)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_ERR, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetDI)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_DI, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetDO)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_DO, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetAI)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_AI, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetAO)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_AO, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetPO)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_PO, (para_t)1, (para_t)0, NEXT_TICK);
                }
                else if (ReturnMode == eRetHelp)
                {
                    PutFiFoSend((uint8_t *)"\r\n", 2);
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_HELP, (para_t)1, (para_t)0, NEXT_TICK);
                }
            }
            break;
        
        case SIG_RPT_CMD_SYS :
        // if FiFo is empty then do his job
        // if not then wait until Fifo is empty
        if (Is_FiFoSend_Empty())
        {
            ReturnMode = RepeatCmdSys(sendBuffer, Para1);
            if (ReturnMode == eRetComplete)
            {
                PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                char_t *Prompt = getPrompt();
                PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
            }
            else if (ReturnMode == eRetSys)
            {
                PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_SYS, (para_t)Para1+1, (para_t)0, NEXT_TICK);
            }
        }
        else
        {
            VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_SYS, Para1, (para_t)0, NEXT_TICK);
        }
        break;
        
        case SIG_RPT_CMD_VER :
        // if FiFo is empty then do his job
        // if not then wait until Fifo is empty
        if (Is_FiFoSend_Empty())
        {
            ReturnMode = RepeatCmdVer(sendBuffer, Para1);
            if (ReturnMode == eRetComplete)
            {
                PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                char_t *Prompt = getPrompt();
                PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
            }
            else if (ReturnMode == eRetVer)
            {
                PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_VER, (para_t)Para1+1, (para_t)0, NEXT_TICK);
            }
        }
        else
        {
            VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_VER, Para1, (para_t)0, NEXT_TICK);
        }
        break;
        
        case SIG_RPT_CMD_ERR :
            // if FiFo is empty then do his job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdErr(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    char_t *Prompt = getPrompt();
                    PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
                }
                else if (ReturnMode == eRetErr)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_ERR, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_ERR, Para1, (para_t)0, NEXT_TICK);
            }
            break;
        
        case SIG_RPT_CMD_DI :
            // if FiFo is empty then do his job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdDI(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    char_t *Prompt = getPrompt();
                    PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
                }
                else if (ReturnMode == eRetDI)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_DI, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_DI, Para1, (para_t)0, NEXT_TICK);
            }
            break;
        
        case SIG_RPT_CMD_DO :
            // if FiFo is empty then do his job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdDO(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    char_t *Prompt = getPrompt();
                    PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
                }
                else if (ReturnMode == eRetDO)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_DO, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_DO, Para1, (para_t)0, NEXT_TICK);
            }
            break;
        
        case SIG_RPT_CMD_AI :
            // if FiFo is empty then do his job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdAI(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    char_t *Prompt = getPrompt();
                    PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
                }
                else if (ReturnMode == eRetDI)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_AI, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_AI, Para1, (para_t)0, NEXT_TICK);
            }
            break;
        

#ifdef HAL_ANAOUT_H_
        case SIG_RPT_CMD_AO :
            // if FiFo is empty then do this job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdAO(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    char_t *Prompt = getPrompt();
                    PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
                }
                else if (ReturnMode == eRetAO)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_AO, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_AO, Para1, (para_t)0, NEXT_TICK);
            }
            break;
#endif // #ifdef HAL_ANAOUT_H_

#ifdef HAL_PWMOUT_H_
        case SIG_RPT_CMD_PO :
            // if FiFo is empty then do this job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdPO(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    char_t *Prompt = getPrompt();
                    PutFiFoSend((uint8_t *)Prompt, strlen((char *)Prompt));
                }
                else if (ReturnMode == eRetPO)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_PO, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_PO, Para1, (para_t)0, NEXT_TICK);
            }
            break;
#endif // #ifdef HAL_PWMOUT_H_

        case SIG_RPT_CMD_HELP :
            // if FiFo is empty then do his job
            // if not then wait until Fifo is empty
            if (Is_FiFoSend_Empty())
            {
                ReturnMode = RepeatCmdHelp(sendBuffer, Para1);
                if (ReturnMode == eRetComplete)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    PutFiFoSend((uint8_t *)sPrompt, strlen((char *)sPrompt));
                }
                else if (ReturnMode == eRetHelp)
                {
                    PutFiFoSend((uint8_t *)sendBuffer, strlen((char *)sendBuffer));
                    VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_HELP, (para_t)Para1+1, (para_t)0, NEXT_TICK);
                }
            }
            else
            {
                VR_RTOS_SendMsg(SHELL_PRIO, MyTaskNo, MyTaskNo, SIG_RPT_CMD_HELP, Para1, (para_t)0, NEXT_TICK);
            }
            break;
        
        default :
            break;
    }
}

static eRetMode_t CmdVer(uint8_t *txtPara, uint8_t *sndBuffer)
{
    VR_StrCpy((char *)sndBuffer, APPL_NAME, "  ", APPL_VERSION, "\r\n", NULL);
    return (eRetVer);
}

static eRetMode_t RepeatCmdVer(uint8_t *sndBuffer, uint8_t idx)
{
    switch (idx)
    {
        case 1 :
            VR_StrCpy((char *)sndBuffer, VR_RTOS_SCHEDULER_VERSION, "\r\n", NULL);
            return (eRetVer);
            break;
        case 2 :
            VR_StrCpy((char *)sndBuffer, VR_RTOS_HAL_VERSION, "\r\n", NULL);
            return (eRetVer);
            break;
        case 3 :
            VR_StrCpy((char *)sndBuffer, SHELL_VERSION, "\r\n", NULL);
            return (eRetComplete);
            break;
    }

    return (eRetComplete);
}

static eRetMode_t CmdSys(uint8_t *txtPara, uint8_t *sndBuffer)
{
    char StrNumber[9];
    char StrPercent[9];
    VR_RTOS_TaskDetail_t *TaskDetail = VR_RTOS_getTaskDetail(0);

    SumPercent = 0;
    sndBuffer[0] = 0;
    VR_Itoa(0, StrNumber);
    VR_Itoa(TaskDetail->PercentTime, StrPercent);
    SumPercent += TaskDetail->PercentTime;
    VR_StrCat((char *)sndBuffer, "task ", StrNumber, " : ", TaskDetail->TaskName, " ", TaskDetail->Task_On ? "on " : "off ", NULL);
    VR_StrCat((char *)sndBuffer, StrPercent, "%\r\n", NULL);
    
    return (eRetSys);
}

static eRetMode_t RepeatCmdSys(uint8_t *sndBuffer, uint8_t idx)
{
    char StrMsg[21];
    char StrNumber[9];
    uint32_t LostMsg;
    uint32_t MsgOverTime;
    char StrPercent[9];
    uint8_t maxTask;
    uint32_t ticks;
    
    maxTask = VR_RTOS_getMaxTask();

    sndBuffer[0] = 0;
    if (idx < maxTask)
    {
        VR_RTOS_TaskDetail_t *TaskDetail = VR_RTOS_getTaskDetail(idx);
        VR_Itoa(idx, StrNumber);
        VR_Itoa(TaskDetail->PercentTime, StrPercent);
        SumPercent += TaskDetail->PercentTime;
        VR_StrCat((char *)sndBuffer, "task ", StrNumber, " : ", TaskDetail->TaskName, " ", TaskDetail->Task_On ? "on " : "off ", NULL);
        VR_StrCat((char *)sndBuffer, StrPercent, "%\r\n", NULL);
        return (eRetSys);
    }
    else if (idx == maxTask)
    {
        VR_Itoa(SumPercent, StrPercent);
        VR_StrCat((char *)sndBuffer, "CPU Load complete : ", StrPercent, "%\r\n", NULL);
        return (eRetSys);
    }
    else if (idx == (maxTask+1))
    {
        VR_StrCat((char *)sndBuffer, "System Tick : ", VR_RTOS_Get_SysTick_String(), "\r\n", NULL);
        return (eRetSys);
    }
    else if (idx == (maxTask+2))
    {
        ticks = VR_RTOS_getTicksSinceStart();
        VR_StrCat((char *)sndBuffer, "Ticks since PowOn : ", getStrTicksSinceStart(ticks), "\r\n", NULL);
        VR_StrCat((char *)sndBuffer, "Working hours : ", getStrWorkingTime(WorkingTime), NULL);
        return (eRetSys);
    }
    else
    {
        LostMsg = VR_RTOS_get_Lost_Messages();
        VR_Itoa(LostMsg, StrMsg);
        VR_StrCat((char *)sndBuffer, "Lost messages : ", StrMsg, "\r\n", NULL);
        
        MsgOverTime = VR_RTOS_getMessagesOverTime();
        VR_Itoa(MsgOverTime, StrMsg);
        VR_StrCat((char *)sndBuffer, "Messages OverTime : ", StrMsg, "\r\n", NULL);
        return (eRetComplete);
    }
}

static eRetMode_t CmdErr(uint8_t *txtPara, uint8_t *sndBuffer)
{
    char *strSig;
    char StrTaskNo[11];
    char StrTaskSig[11];

    VR_RTOS_Error_t *Error = getErrorManager_Error(0);

    if (Error != NULL)
    {
        switch (Error->ErrorSig)
        {
            case SIG_ERR_UNKNOWN  : strSig = "ERR_UNKNOWN : "; break;
            case SIG_ERR_SYS      : strSig = "ERR_SYS : "; break;
            case SIG_ERR_LOST_MSG : strSig = "SIG_ERR_LOST_MSG : "; break;
            case SIG_ERR_OVERTIME : strSig = "ERR_OVERTIME : "; break;
            case SIG_ERR_COMMON   : strSig = "ERR_COMMON : "; break;
            default :               strSig = "ERR_UNKNOWN : "; break;
        }
        VR_Itoa(Error->TaskNo, StrTaskNo);
        VR_Itoa(Error->TaskSig, StrTaskSig);
        VR_StrCat((char *)sndBuffer, strSig, "Task ", StrTaskNo, "Sig ", StrTaskSig, NULL);
        VR_StrCat((char *)sndBuffer, " Tick since start ", getStrTicksSinceStart(Error->TicksSinceStart), "Seconds sincePowOn ", NULL);
        VR_StrCat((char *)sndBuffer, "\r\n", NULL);
        return (eRetErr);
    }
    
    return (eRetComplete);
}

static eRetMode_t RepeatCmdErr(uint8_t *sndBuffer, uint8_t idx)
{
    char *strSig;
    VR_RTOS_Error_t *Error = getErrorManager_Error(idx);
    if (Error != NULL)
    {
        switch (Error->ErrorSig)
        {
            case SIG_ERR_UNKNOWN  : strSig = "ERR_UNKNOWN : "; break;
            case SIG_ERR_SYS      : strSig = "ERR_SYS : "; break;
            case SIG_ERR_LOST_MSG : strSig = "SIG_ERR_LOST_MSG : "; break;
            case SIG_ERR_OVERTIME : strSig = "ERR_OVERTIME : "; break;
            default :               strSig = "ERR_UNKNOWN : "; break;
        }
        VR_StrCat((char *)sndBuffer, strSig, "Tick since PowOn ", getStrTicksSinceStart(Error->TicksSinceStart), "Seconds since PowOn ", NULL);
        VR_StrCat((char *)sndBuffer, "\r\n", NULL);
        return (eRetErr);
    }
    
    return (eRetComplete);
}

static eRetMode_t CmdStart(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int idx;

    if (isdigit(txtPara[0]))
    {
        idx = atoi((const char *)txtPara);
        VR_RTOS_Start_Task(idx);
        VR_StrCpy((char *)sndBuffer, "task: ", VR_RTOS_Get_TaskName(idx), " on\r\n", NULL);
    }
    return (eRetComplete);
}

static eRetMode_t CmdStop(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int idx;

    if (isdigit(txtPara[0]))
    {
        idx = atoi((const char *)txtPara);
        if (idx != MyTaskNo)
        {
            VR_RTOS_Stop_Task(idx);
            VR_StrCpy((char *)sndBuffer, "task: ", VR_RTOS_Get_TaskName(idx), " off\r\n", NULL);
        }
        else
        {
            VR_StrCpy((char *)sndBuffer, "task: can not stop ", VR_RTOS_Get_TaskName(idx), "\r\n", NULL);
        }
    }
    return (eRetComplete);
}

static eRetMode_t CmdDI(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint8_t maxDigIn = HAL_get_MaxDigIn();
    
    if (maxDigIn > 0)
    VR_StrCpy((char *)sndBuffer, "DIn 0 ", HAL_Get_DigIn_Name(0), " : ", HAL_Get_DigIn(0) ? "1" : "0", "\r\n", NULL);
    else
    VR_StrCpy((char *)sndBuffer, "No DIn exist\r\n", NULL);

    if (maxDigIn <= 1)
    return (eRetComplete);
    else
    return (eRetDI);
}

static eRetMode_t RepeatCmdDI(uint8_t *sndBuffer, uint8_t index)
{
    char no[4];
    char val[8];
    uint8_t maxDigIn = HAL_get_MaxDigIn();
    
    VR_Itoa(HAL_Get_DigIn(index), val);
    VR_Itoa(index, no);
    VR_StrCpy((char *)sndBuffer, "DIn ", no, " ", HAL_Get_DigIn_Name(index), " : ", val, "\r\n", NULL);

    if (index == (maxDigIn-1))
    return (eRetComplete);
    else
    return (eRetDI);
}

static eRetMode_t CmdDIs(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int i;

    if (isdigit(txtPara[0]))
    {
        i = atoi((char*)txtPara);
        if (i < HAL_get_MaxDigIn())
        {
            Set_DigIn_Test(i, TRUE);
            VR_StrCpy((char *)sndBuffer, "DIn ", HAL_Get_DigIn_Name(i), " : ", HAL_Get_DigIn(i) ? "1" : "0", "\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdDIr(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int i;

    if (isdigit(txtPara[0]))
    {
        i = atoi((char*)txtPara);
        if (i < HAL_get_MaxDigIn())
        {
            Set_DigIn_Test(i, FALSE);
            VR_StrCpy((char *)sndBuffer, "DIn ", HAL_Get_DigIn_Name(i), " : ", HAL_Get_DigIn(i) ? "1" : "0", "\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdDIt(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int i;

    if (isdigit(txtPara[0]))
    {
        i = atoi((char*)txtPara);
        if (i < HAL_get_MaxDigIn())
        {
            HAL_Get_DigIn(i) ? Set_DigIn_Test(i, FALSE) : Set_DigIn_Test(i, TRUE);
            VR_StrCpy((char *)sndBuffer, "DIn ", HAL_Get_DigIn_Name(i), " : ", HAL_Get_DigIn(i) ? "1" : "0", "\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdDO(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint8_t maxDigOut = HAL_get_MaxDigOut();
    
    if (maxDigOut > 0)
    VR_StrCpy((char *)sndBuffer, "DOut 0 ", HAL_Get_DigOut_Name(0), " : ", HAL_Get_DigOut(0) ? "1" : "0", "\r\n", NULL);
    else
    VR_StrCpy((char *)sndBuffer, "No DOut exist\r\n", NULL);

    if (maxDigOut <= 1)
    return (eRetComplete);
    else
    return (eRetDO);
}

static eRetMode_t RepeatCmdDO(uint8_t *sndBuffer, uint8_t index)
{
    char no[4];
    uint8_t maxDigOut = HAL_get_MaxDigOut();
    
    VR_Itoa(index, no);
    VR_StrCpy((char *)sndBuffer, "DOut ", no, " ", HAL_Get_DigOut_Name(index), " : ", HAL_Get_DigOut(index) ? "1" : "0", "\r\n", NULL);

    if (index == (maxDigOut-1))
    return (eRetComplete);
    else
    return (eRetDO);
}

static eRetMode_t CmdDOs(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int i;

    if (isdigit(txtPara[0]))
    {
        i = atoi((char*)txtPara);
        if (i < HAL_get_MaxDigOut())
        {
            Set_DigOut_Manu(i);
            VR_StrCpy((char *)sndBuffer, "DOut ", HAL_Get_DigOut_Name(i), " : ", HAL_Get_DigOut(i) ? "1" : "0", "\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdDOr(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int i;

    if (isdigit(txtPara[0]))
    {
        i = atoi((char*)txtPara);
        if (i < HAL_get_MaxDigOut())
        {
            Reset_DigOut_Manu(i);
            VR_StrCpy((char *)sndBuffer, "DOut ", HAL_Get_DigOut_Name(i), " : ", HAL_Get_DigOut(i) ? "1" : "0", "\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdDOt(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int i;

    if (isdigit(txtPara[0]))
    {
        i = atoi((char*)txtPara);
        if (i < HAL_get_MaxDigOut())
        {
            Toggle_DigOut_Manu(i);
            VR_StrCpy((char *)sndBuffer, "DOut ", HAL_Get_DigOut_Name(i), " : ", HAL_Get_DigOut(i) ? "1" : "0", "\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdRst(uint8_t *txtPara, uint8_t *sndBuffer)
{
    // watchdog must perform while "delay_ms(100)"
    //delay_ms(100);

    VR_StrCpy((char *)sndBuffer, "Command rst not implemented\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdWDog(uint8_t *txtPara, uint8_t *sndBuffer)
{
    // watchdog must perform while "delay_ms(100)"
    //delay_ms(100);

    VR_StrCpy((char *)sndBuffer, "Command wdog not implemented\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdHelp(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (CmdTab[0].lenCommand != 0)
    {
        if (strchr((char*)CmdTab[0].pTxtFlag, 'A') || strchr((char*)CmdTab[0].pTxtFlag, ModeChar[actualMode]))
        VR_StrCpy((char *)sndBuffer, CmdTab[0].pTxtCommand, " : ", CmdTab[0].pTxtHelp, "\r\n", NULL);
    }
    
    return (eRetHelp);
}

static eRetMode_t RepeatCmdHelp(uint8_t *sndBuffer, uint8_t idx)
{
    eRetMode_t ret = eRetComplete;
    
    sndBuffer[0] = 0;
    if (CmdTab[idx].lenCommand > 0)
    {
        if (strchr((char*)CmdTab[idx].pTxtFlag, 'A') || strchr((char*)CmdTab[idx].pTxtFlag, ModeChar[actualMode]))
        VR_StrCpy((char *)sndBuffer, CmdTab[idx].pTxtCommand, " : ", CmdTab[idx].pTxtHelp, "\r\n", NULL);
        ret = eRetHelp;
    }
    return (ret);
}

static eRetMode_t interpretRcvBuffer(uint8_t *pRcvBuf, uint8_t *sndBuffer)
{
    static uint8_t *pCmd=NULL;   // pinter to command string
    static uint8_t *pParam=NULL; // pinter to first parameter in command string
    
    uint8_t lenCmd=0;
    uint8_t i;
    uint8_t ch;

    if (pRcvBuf == NULL)
    {
        VR_StrCpy((char *)sndBuffer, "Command not found.\r\n", NULL);
        return (eRetComplete);
    }

    if (*pRcvBuf == 0)
    {
        VR_StrCpy((char *)sndBuffer, "Command not found.\r\n", NULL);
        return (eRetComplete);
    }

    // Convert command string to lower
    i = 0;
    while (pRcvBuf[i])
    {
        pRcvBuf[i] = tolower(pRcvBuf[i]);
        i++;
    }

    // look for first occurence of a letter
    i = 0;
    while (pRcvBuf[i])
    {
        ch = pRcvBuf[i];
        if ((ch >= 'a') && (ch <= 'z'))
        {
            pCmd = &pRcvBuf[i];
            break;
        }
        i++;
    }

    if (!pCmd) // no command found !!!
    {
        VR_StrCpy((char *)sndBuffer, "Command not found.\r\n", NULL);
        return (eRetComplete);
    }

    // command found, now find out how long is the command
    i = 1;
    lenCmd = 1;
    while (pCmd[i])
    {
        ch = pRcvBuf[i];
        if (((ch >= 'a') && (ch <= 'z')) ||	((ch >= '0') && (ch <= '9')))
        {
            lenCmd++;
            i++;
        }
        else
        break;
    }

    // look for first occurence of a letter after command string
    while (pCmd[i])
    {
        ch = pRcvBuf[i];
        if (((ch >= 'a') && (ch <= 'z')) || ((ch >= '0') && (ch <= '9')))
        {
            pParam = &pCmd[i];
            break;
        }
        i++;
    }

    //Find a command function
    i = 0;
    while (CmdTab[i].lenCommand)
    {
        if (CmdTab[i].lenCommand == lenCmd)
        {
            char s[10];
            char t[10];
            strcpy(s, (const char *)CmdTab[i].pTxtCommand);
            strcpy(t, (const char *)pCmd);
            if (memcmp((const char *)CmdTab[i].pTxtCommand, (const char *)pCmd, lenCmd) == 0)
            {
                return (CmdTab[i].pFunc(pParam, sndBuffer));
            }
        }
        i++;
    }

    // no command function found
    VR_StrCpy((char *)sndBuffer, "Command not found.\r\n", NULL);
    return (eRetComplete);
}

static void stopRepeat()
{
    repDoMax=0; repDiMax=0; repAiMax=0;
    repDoCnt=0; repDiCnt=0; repAiCnt=0;
}

char_t *getPrompt(void)
{
    static char strgPrompt[31];
    
    switch (actualMode)
    {
        case eModeBase  :
            VR_StrCpy((char *)strgPrompt, sPrompt, NULL);
            break;
        case eModeExpert :
            VR_StrCpy((char *)strgPrompt, sExpertPrompt, sPrompt, NULL);
            break;
        case eModeFactory :
            VR_StrCpy((char *)strgPrompt, sFactoryPrompt, sPrompt, NULL);
            break;
        case eModeManual :
            VR_StrCpy((char *)strgPrompt, sManualPrompt, sPrompt, NULL);
            break;
        case eModeTest :
            VR_StrCpy((char *)strgPrompt, sTestPrompt, sPrompt, NULL);
            break;
        case eModeEMV :
            VR_StrCpy((char *)strgPrompt, sEMVVPrompt, sPrompt, NULL);
            break;
    }

    return ((char_t *)strgPrompt);
}

static eRetMode_t CmdExptOn(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeExpert;
    VR_StrCpy((char *)sndBuffer, "Expert mode on\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdExptOff(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeBase;
    VR_StrCpy((char *)sndBuffer, "Expert mode off\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdFactOn(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeFactory;
    VR_StrCpy((char *)sndBuffer, "Factory mode on\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdFactOff(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeBase;
    VR_StrCpy((char *)sndBuffer, "Factory mode off\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdManuOn(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeManual;
    VR_StrCpy((char *)sndBuffer, "Manual mode on\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdManuOff(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeBase;
    VR_StrCpy((char *)sndBuffer, "Manual mode off\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdTestOn(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeTest;
    VR_StrCpy((char *)sndBuffer, "Test mode on\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdTestOff(uint8_t *txtPara, uint8_t *sndBuffer)
{
    actualMode = eModeBase;
    VR_StrCpy((char *)sndBuffer, "Test mode off\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdTrace(uint8_t *txtPara, uint8_t *sndBuffer)
{
    int idx;
    bool ok=false;

    if (isdigit(txtPara[0]))
    {
        idx = atoi((char *)txtPara);
        ok = true;
    }
    
    if (ok)
    {
        VR_RTOS_SendMsg(PRIO_LOW, MyTaskNo, idx, SIG_TRACE_TOGGLE, 0, 0, NO_TICK);
        VR_StrCpy((char *)sndBuffer, "trace of task ", VR_RTOS_Get_TaskName(idx), " has toggled\r\n", NULL);
    }
    else
    {
        VR_RTOS_SendMsg_Broadcast(PRIO_LOW, SIG_TRACE_OFF, 0, 0);
        VR_StrCpy((char *)sndBuffer, "trace of any task is off\r\n", NULL);
    }
    return (eRetComplete);
}

static eRetMode_t CmdAI(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint8_t i=0;
    char no[4];
    char val[8];

    uint8_t maxAnaIn = HAL_get_MaxAnaIn();
    
    sndBuffer[0] = 0;
    VR_Itoa(HAL_Get_AnaIn_Value(i), val);
    VR_Itoa(i, no);
    VR_StrCat((char *)sndBuffer, "AIn ", no, " ", HAL_Get_AnaIn_Name(i), " ", val, "\r\n", NULL);

    if (maxAnaIn == 0)
        return (eRetComplete);
    else
        return (eRetAI);
}

static eRetMode_t CmdAIs(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint16_t anaInCh;
    uint16_t anaInVal;
    char no[4];
    char val[8];

    uint8_t maxAnaIn = HAL_get_MaxAnaIn();

    if (isdigit(txtPara[0]))
    {
        anaInCh = atoi((char*)txtPara);
        
        // searching end of first parameter
        while ((*txtPara >= '0') && (*txtPara <= '9'))
        txtPara++;

        // step to next parameter
        if (*txtPara > 0)
        txtPara++;
        if (*txtPara > 0)
        {
            anaInVal = atoi((char*)txtPara);
            if (anaInCh < maxAnaIn)
            {
                Set_AnaIn_Test(anaInCh, anaInVal);
                VR_Itoa(anaInCh, no);
                VR_Itoa(HAL_Get_AnaIn_Value(anaInCh), val);
                VR_StrCpy((char *)sndBuffer, "AnaIn ", no, " ", HAL_Get_AnaIn_Name(anaInCh), " : ", val, "\r\n", NULL);
            }
            else
            VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
        }
        else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
    VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

#ifdef HAL_ANAOUT_H_
static eRetMode_t CmdAO(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint8_t i=0;
    char no[4];
    char val[8];

    uint8_t maxAnaOut = HAL_get_MaxAnaOut();
    
    sndBuffer[0] = 0;
    VR_Itoa(HAL_Get_AnaOut(i), val);
    VR_Itoa(i, no);
    VR_StrCat((char *)sndBuffer, "AOut ", no, " ", HAL_Get_AnaOut_Name(i), " ", val, "\r\n", NULL);

    if (maxAnaOut == 0)
        return (eRetComplete);
    else
        return (eRetAO);
}

static eRetMode_t CmdAOs(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint16_t anaOutCh;
    uint16_t anaOutVal;
    char no[4];
    char val[8];

    uint8_t maxAnaOut = HAL_get_MaxAnaOut();

    if (isdigit(txtPara[0]))
    {
        anaOutCh = atoi((char*)txtPara);
        
        // searching end of first parameter
        while ((*txtPara >= '0') && (*txtPara <= '9'))
        txtPara++;

        // step to next parameter
        if (*txtPara > 0)
            txtPara++;
        if (*txtPara > 0)
        {
            anaOutVal = atoi((char*)txtPara);
            if (anaOutCh < maxAnaOut)
            {
                Set_AnaOut_Manu(anaOutCh, anaOutVal);
                VR_Itoa(anaOutCh, no);
                VR_Itoa(HAL_Get_AnaOut_Value(anaOutCh), val);
                VR_StrCpy((char *)sndBuffer, "AnaOut ", no, " ", HAL_Get_AnaOut_Name(anaOutCh), " : ", val, "\r\n", NULL);
            }
            else
                VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
        }
        else
            VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}
#endif // #ifdef HAL_ANAOUT_H_

static eRetMode_t RepeatCmdAI(uint8_t *sndBuffer, uint8_t index)
{
    char no[4];
    char val[8];

    uint8_t maxAnaIn = HAL_get_MaxAnaIn();
    
    sndBuffer[0] = 0;
    VR_Itoa(HAL_Get_AnaIn_Value(index), val);
    VR_Itoa(index, no);
    VR_StrCat((char *)sndBuffer, "AIn ", no, " ", HAL_Get_AnaIn_Name(index), " ", val, "\r\n", NULL);

    if (index == (maxAnaIn - 1))
        return (eRetComplete);
    else
        return (eRetAI);
}

#ifdef HAL_ANAOUT_H_
static eRetMode_t RepeatCmdAO(uint8_t *sndBuffer, uint8_t index)
{
    char no[4];
    char val[8];

    uint8_t maxAnaOut = HAL_get_MaxAnaOut();
    
    sndBuffer[0] = 0;
    VR_Itoa(HAL_Get_AnaOut(index), val);
    VR_Itoa(index, no);
    VR_StrCat((char *)sndBuffer, "AOut ", no, " ", HAL_Get_AnaOut_Name(index), " ", val, "\r\n", NULL);

    if (index == (maxAnaOut - 1))
        return (eRetComplete);
    else
        return (eRetAO);
}
#endif // #ifdef HAL_ANAOUT_H_
#ifdef HAL_PWMOUT_H_
static eRetMode_t CmdPOs(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint16_t pwmOutCh;
    uint16_t pwmOutVal;
    char no[4];
    char val[8];

    uint8_t maxPwmOut = HAL_get_MaxPwmOut();

    if (isdigit(txtPara[0]))
    {
        pwmOutCh = atoi((char*)txtPara);
        
        // searching end of first parameter
        while ((*txtPara >= '0') && (*txtPara <= '9'))
            txtPara++;

        // step to next parameter
        if (*txtPara > 0)
            txtPara++;
        if (*txtPara > 0)
        {
            pwmOutVal = atoi((char*)txtPara);
            if (pwmOutCh < maxPwmOut)
            {
                Set_PwmOut_Manu(pwmOutCh, pwmOutVal);
                VR_Itoa(pwmOutCh, no);
                VR_Itoa(HAL_Get_PwmOut_Value(pwmOutCh), val);
                VR_StrCpy((char *)sndBuffer, "PwmOut ", no, " ", HAL_Get_PwmOut_Name(pwmOutCh), " : ", val, "\r\n", NULL);
            }
            else
                VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
        }
        else
            VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);
    }
    else
        VR_StrCpy((char *)sndBuffer, "No valid parameter\r\n", NULL);

    return (eRetComplete);
}

static eRetMode_t CmdPO(uint8_t *txtPara, uint8_t *sndBuffer)
{
    uint8_t i=0;
    char no[4];
    char val[8];

    uint8_t maxPwmOut = HAL_get_MaxPwmOut();
    
    sndBuffer[0] = 0;
    VR_Itoa(HAL_Get_PwmOut_Value(i), val);
    VR_Itoa(i, no);
    VR_StrCat((char *)sndBuffer, "AOut ", no, " ", HAL_Get_PwmOut_Name(i), " ", val, "\r\n", NULL);

    if (maxPwmOut == 0)
        return (eRetComplete);
    else
        return (eRetPO);
}
static eRetMode_t RepeatCmdPO(uint8_t *sndBuffer, uint8_t index)
{
    char no[4];
    char val[8];

    uint8_t maxPwmOut = HAL_get_MaxPwmOut();
    
    sndBuffer[0] = 0;
    VR_Itoa(HAL_Get_PwmOut_Value(index), val);
    VR_Itoa(index, no);
    VR_StrCat((char *)sndBuffer, "POut ", no, " ", HAL_Get_PwmOut_Name(index), " ", val, "\r\n", NULL);

    if (index == (maxPwmOut - 1))
        return (eRetComplete);
    else
        return (eRetPO);
}
#endif // #ifdef HAL_PWMOUT_H_

static eRetMode_t CmdRepDO(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (isdigit(txtPara[0]))
    {
        repDoMax = atoi((char*)txtPara);
    }
    else
        repDoMax = 10;

    return (eRetComplete);
}

static eRetMode_t CmdRepDI(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (isdigit(txtPara[0]))
    {
        repDiMax = atoi((char*)txtPara);
    }
    else
    repDiMax = 10;

    return (eRetComplete);
}

static eRetMode_t CmdRepAI(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (isdigit(txtPara[0]))
    {
        repAiMax = atoi((char*)txtPara);
    }
    else
    {
        repAiMax = 10;
    }

    return (eRetComplete);
}

#ifdef HAL_ANAOUT_H_
static eRetMode_t CmdRepAO(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (isdigit(txtPara[0]))
    {
        repAoMax = atoi((char*)txtPara);
    }
    else
    {
        repAoMax = 10;
    }

    return (eRetComplete);
}
#endif // #ifdef HAL_ANAOUT_H_

#ifdef HAL_PWMOUT_H_
static eRetMode_t CmdRepPO(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (isdigit(txtPara[0]))
    {
        repPoMax = atoi((char*)txtPara);
    }
    else
    {
        repPoMax = 10;
    }

    return (eRetComplete);
}
#endif // #ifdef HAL_PWMOUT_H_
static eRetMode_t CmdRepAll(uint8_t *txtPara, uint8_t *sndBuffer)
{
    if (isdigit(txtPara[0]))
    {
        repDoMax = atoi((char*)txtPara);
    }
    else
    repDoMax = 10;
    repDiMax = repDoMax;
    repAiMax = repDoMax;
    
    return (eRetComplete);
}

static eRetMode_t CmdX(uint8_t *txtPara, uint8_t *sndBuffer)
{
    stopRepeat();

    return (eRetComplete);
}

static void doRepDO(uint8_t *sndBuffer)
{
    uint8_t i=0;

    strcat((char *)sndBuffer, "DigOut :");
    
    for (i=0; i < HAL_get_MaxDigOut(); i++)
    {
        if ((i%4) == 0)
            strcat((char *)sndBuffer, " ");
        
        if (HAL_Get_DigOut(i))
            strcat((char *)sndBuffer, "1");
        else
            strcat((char *)sndBuffer, "0");
    }
    strcat((char *)sndBuffer, "\r\n");
}

static void doRepDI(uint8_t *sndBuffer)
{
    uint8_t i=0;

    strcat((char *)sndBuffer, "DigIn :");
    
    for (i=0; i<HAL_get_MaxDigIn(); i++)
    {
        if ((i%4) == 0)
            strcat((char *)sndBuffer, " ");
        
        if (HAL_Get_DigIn(i))
            strcat((char *)sndBuffer, "1");
        else
            strcat((char *)sndBuffer, "0");
    }
    strcat((char *)sndBuffer, "\r\n");
}

static void doRepAI(uint8_t *sndBuffer)
{
    uint8_t i=0;
    char str[11];

    strcat((char *)sndBuffer, "AnaIn :");
    
    for (i=0; i<HAL_get_MaxAnaIn(); i++)
    {
        VR_Itoa(HAL_Get_AnaIn_Value(i), str);
        VR_StrCat((char *)sndBuffer, " ", str, NULL);
    }
    strcat((char *)sndBuffer, "\r\n");
}

#ifdef HAL_ANAOUT_H_
static void doRepAO(uint8_t *sndBuffer)
{
    uint8_t i=0;
    char str[11];

    strcat((char *)sndBuffer, "AnaIn :");
    
    for (i=0; i<HAL_get_MaxAnaOut(); i++)
    {
        VR_Itoa(HAL_Get_AnaOut_Value(i), str);
        VR_StrCat((char *)sndBuffer, " ", str, NULL);
    }
    strcat((char *)sndBuffer, "\r\n");
}
#endif // #ifdef HAL_ANAOUT_H_

#ifdef HAL_PWMOUT_H_
static void doRepPO(uint8_t *sndBuffer)
{
    uint8_t i=0;
    char str[11];

    strcat((char *)sndBuffer, "AnaIn :");
    
    for (i=0; i<HAL_get_MaxPwmOut(); i++)
    {
        VR_Itoa(HAL_Get_PwmOut_Value(i), str);
        VR_StrCat((char *)sndBuffer, " ", str, NULL);
    }
    strcat((char *)sndBuffer, "\r\n");
}
#endif // #ifdef HAL_PWMOUT_H_

static void PutFiFoSend(uint8_t * buffer, uint8_t size)
{
    if (TaskNo_To_Send == RS232_TASKNO)
    {
        VR_PutFiFo_Size(&RS232_Send_FiFo, buffer, size);
    }
}

static bool Is_FiFoSend_Empty(void)
{
    if (TaskNo_To_Send == RS232_TASKNO)
    {
        return (VR_Is_FiFo_Empty(&RS232_Send_FiFo));
    }

    return (false);
}

static char* getStrWorkingTime(WorkingTime_t WorkingTime)
{
    static char StrMsg[61];
    char StrSec[5];
    char StrMin[5];
    char StrHour[11];
    uint16_t hours;
    uint8_t min;
    uint8_t seconds;
    
    hours = WorkingTime.Hour;
    min = WorkingTime.Minutes;
    seconds = WorkingTime.Seconds;
    if (seconds < 10)
    {
        StrSec[0] = '0';
        StrSec[1] = '0' + seconds;
        StrSec[2] = 0;
    }
    else
        VR_Itoa(seconds, StrSec);

    if (min < 10)
    {
        StrMin[0] = '0';
        StrMin[1] = '0' + min;
        StrMin[2] = 0;
    }
    else
        VR_Itoa(min, StrMin);

    VR_Itoa(hours, StrHour);

    VR_StrCpy(StrMsg, StrHour, ":", StrMin, ":", StrSec, "\r\n", NULL);

    return (StrMsg);
}

static char* getStrTicksSinceStart(uint32_t ticks)
{
    static char StrTicks[21];

    VR_Itoa(ticks, StrTicks);

    return (StrTicks);
}

bool Is_ShellMode_Test(void)
{
    return (actualMode == eModeTest);
}

bool Is_ShellMode_Manu(void)
{
    return (actualMode == eModeManual);
}