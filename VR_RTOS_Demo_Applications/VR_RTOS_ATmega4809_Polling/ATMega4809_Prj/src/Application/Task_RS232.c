#include <atmel_start.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "SCHEDULER/SCHEDULER.h"
#include "VR_TOOLS/VR_FiFo.h"

#define FOSC    8000000 // Clock Speed
#define BAUD    9600
#define MY_UBRR FOSC/16/BAUD-1

#define CR                 13
#define LF                 10
#define SEND_BUFFER_SIZE   256
#define RECV_BUFFER_SIZE   64

static void performRcvData(uint8_t data);


static uint8_t s_rcvBuffer[RECV_BUFFER_SIZE];
static uint8_t s_rcvBufferRepeat[RECV_BUFFER_SIZE];
static uint8_t s_rcvBufferIdx=0;
static uint8_t s_TaskShellNo=INVALID_TASKNO;
static uint8_t s_TaskLedNo=INVALID_TASKNO;

static uint8_t sndBuff[SEND_BUFFER_SIZE];


VR_FiFo_t RS232_Send_FiFo; // Exception: global variable; used in Shell.c

void Send_Trace(uint8_t Task_From, char * TraceString);
void send_Prompt(void);

extern char_t *getPrompt(void); // Task_Shell.c

void init_RS232(void)
{  
	// Init of UART is done in main()
	
	//uint16_t MyUBRR = MY_UBRR;
	//
	///* Enable USART0 */
	//PRR0 &= ~(1 << PRUSART0);
	//
	//UCSR0A = 0;
	//
	//// Enable receiver and transmitter
	//UCSR0B = (1<<RXEN0) | (1<<TXEN0);
	//
	//// Set frame format: 8data, 1stop bit
	//UCSR0C = (0<<USBS0) | (3<<UCSZ00);
	//
	//UBRR0 = MyUBRR;
	
	s_TaskShellNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_SHELL);
    s_TaskLedNo = VR_RTOS_Get_TaskNo((uint8_t *)STR_TASK_LED);

    VR_InitFiFo(&RS232_Send_FiFo, sndBuff, sizeof(sndBuff));
}

void do_Task_RS232(void)
{
    static bool firstCall = true;
	uint8_t sendData = 0;
	uint8_t readData = 0;
	
    if (firstCall)
    {
        firstCall = false;
        init_RS232();
    }

	if (USART_0_is_rx_ready()) // Rx buffer has unread data
	{
		readData = USART_0_read();
		
		// perform echo
		if (!VR_Is_FiFo_Full(&RS232_Send_FiFo))
			VR_PutFiFo(&RS232_Send_FiFo, readData);

        performRcvData(readData);
	}
	
	if (USART_0_is_tx_ready()) // Tx interface empty
	{
		if (!VR_Is_FiFo_Empty(&RS232_Send_FiFo))
		{
			if (VR_GetFiFo(&RS232_Send_FiFo, &sendData))
			{
				USART_0_write(sendData); // send Byte to RS232
			}
		}
	}
}

static void performRcvData(uint8_t data)
{
    if (data != LF)
    {
        if ((data == 0x08) || (data == 0x7F)) // Backspace or Delete
        {
            if (s_rcvBufferIdx > 0)
            {
                s_rcvBufferIdx--;
                s_rcvBuffer[s_rcvBufferIdx] = 0;
            }
        }
        else if (data == CR)
        {
            if (s_rcvBufferIdx == 0)
            {
                if (s_rcvBufferRepeat[0] != 0)
                {
                    // send address of receive buffer to the given task with old content
                    VR_RTOS_SendMsg(PRIO_LOW, RS232_TASKNO, s_TaskShellNo, SIG_RECV, (para_t)s_rcvBufferRepeat, 0, NO_TICK);
                    VR_PutFiFo_Size(&RS232_Send_FiFo, s_rcvBuffer, strlen((char *)s_rcvBuffer));
                    VR_PutFiFo_Size(&RS232_Send_FiFo, (uint8_t *)"\r\n", 2);
                }
            }
            else
            {
                if ((s_rcvBufferIdx == 2) && (s_rcvBuffer[0] == 'L') && (s_rcvBuffer[1] == '+'))
                {
                    VR_RTOS_SendMsg(PRIO_LOW, RS232_TASKNO, s_TaskLedNo, SIG_PLUS_LEDPROG, 0, 0, NO_TICK);
                    VR_PutFiFo_Size(&RS232_Send_FiFo, (uint8_t *)"\r\n", 2);
                    send_Prompt();
                    s_rcvBuffer[s_rcvBufferIdx] = 0;
                    s_rcvBufferIdx = 0;
                }
                else if ((s_rcvBufferIdx == 2) && (s_rcvBuffer[0] == 'L') && (s_rcvBuffer[1] == '-'))
                {
                    VR_RTOS_SendMsg(PRIO_LOW, RS232_TASKNO, s_TaskLedNo, SIG_MINUS_LEDPROG, 0, 0, NO_TICK);
                    VR_PutFiFo_Size(&RS232_Send_FiFo, (uint8_t *)"\r\n", 2);
                    send_Prompt();
                    s_rcvBuffer[s_rcvBufferIdx] = 0;
                    s_rcvBufferIdx = 0;
                }
                else
                {
                    // send address of receive buffer to the shell task
                    VR_RTOS_SendMsg(PRIO_LOW, RS232_TASKNO, s_TaskShellNo, SIG_RECV, (para_t)s_rcvBuffer, 0, NO_TICK);
                    memcpy(s_rcvBufferRepeat, s_rcvBuffer, sizeof(s_rcvBuffer));
                    s_rcvBuffer[s_rcvBufferIdx] = 0;
                    s_rcvBufferIdx = 0;
                }
            }
        }
        else
        {
            s_rcvBuffer[s_rcvBufferIdx] = data;
            s_rcvBufferIdx++;
        }
    }
}

void Send_Trace(uint8_t Task_From, char* TraceString)
{
    if (VR_RTOS_Is_Task_On(s_TaskShellNo) && VR_RTOS_Is_Task_Trace_On(Task_From))
    {
        VR_PutFiFo_Size(&RS232_Send_FiFo, (uint8_t *)TraceString, strlen(TraceString));
        send_Prompt();
    }
}

void send_Prompt()
{
    char_t *Prompt;

    Prompt = getPrompt();
    VR_PutFiFo_Size(&RS232_Send_FiFo, (uint8_t *)Prompt, strlen((char *)Prompt));
}