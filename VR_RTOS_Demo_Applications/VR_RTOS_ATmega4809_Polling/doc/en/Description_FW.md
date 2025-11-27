
## Description FW VR_RTOS_ATmega4809_Polling Demo Project
Datum: 22. Nov 2025

### 1 General
This project uses the VR-RTOS Polling operating system for ATMega 4809 Polling 1.0.1.
The project was developed on the "ATMega4809 Xplained Pro" board.
Microchip Studio 7.0.2595 was used for firmware development.
All task files for the demo are located in the Application folder.


### 2 Board-Driver-Task
The board driver task is located in the file Task_BoardDrv.c.
- The user button is evaluated in the function process_Switch_Inputs(). When pressed, a message SIG_PLUS_LEDPROG is sent to the LED task.
- The analog inputs are read in the function process_AnaIns()
- The PWM output channels are set in the function process_PwmOuts().
- An operating hours counter is implemented in the function inc_VR_RTOS_WorkingTime().


###3 Delay-Task
The board driver task is located in the file Task_Delay.c.
This task simply executes a 1ms delay on each tick.


###4 Error-Manager-Task
The error manager task is located in the file Task_ErrorManager.c.
This task receives and stores error messages.


###5 Led-Task
The LED task is located in the file Task_Led.c.
This task makes the user LED blink.
- Upon receiving the message SIG_PLUS_LEDPROG, the blink time is increased by 100ms. When it reaches 1000ms, it is set to 100ms.
- Upon receiving the message SIG_MINUS_LEDPROG, the blink time is decreased by 100ms. When it reaches 0ms, it is set to 1000ms.
- To make the LED blink, the message SIG_LED_TOGGLE is sent to itself cyclically. The blink time is stored in the variable s_Blink_Time.


###6 Null-Task
The null task is located in the file Task_NULL.c.
This task is used to calculate task times.



###7 RS232-Task
The RS232 task is located in the file Task_RS232.c.
This task belongs to the Scheduler task.
- It receives data from the UART and forwards it to the shell.
- It reads data from the FIFO memory RS232_Send_FiFo and sends it to the UART.
- Exception: If the commands "L+" or "L-" are received from the UART, the messages SIG_PLUS_LEDPROG or SIG_MINUS_LEDPROG, respectively, are sent directly to the LED task.


###8 Shell-Task
The shell task is located in the file Task_Shell.c.
Refer to Description_Shell.md.


###9 Startup-Task
The startup task is located in the file Task_Startup.c.
All tasks are started sequentially within this file. After all tasks have started, the task terminates itself.
