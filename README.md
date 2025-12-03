# VR_RTOS
VR_RTOS is a Real Time Ooperating System for small Microcontroller.
VR_RTOS consists of :
- Scheduler
- Application Tasks (may only call HAL functions)
- HAL functions (are the link between application and hardware)
- HW Driver (may only be called by HAL)
- Polling Version : 
-- No Interrupt inside the system, all registers were polled
-- Only One Timer is used
- ISR Version : 
-- An Interrupt is used for System Timer
-- An additional timer is used for statistics

High Level Features
- Real-time cooperative message based multitasking operating system.
- Programming language: C
- No dependencies on a compiler or CPU
- No dynamic memory is used, only static arrays are used

Supported architectures
- all 8-16-32-Bit-architectures

