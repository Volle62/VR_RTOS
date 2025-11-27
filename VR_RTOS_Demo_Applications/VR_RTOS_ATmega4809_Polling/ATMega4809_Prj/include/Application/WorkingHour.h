#ifndef WORKINGHOUR_H_
#define WORKINGHOUR_H_

#include "SCHEDULER\SCHEDULER.h"

typedef struct
{
    uint16_t Hour;
    uint8_t  Minutes;
    uint8_t  Seconds;
} WorkingTime_t;

extern WorkingTime_t WorkingTime; // global variable => Task_BoardDrv.c

#endif /* WORKINGHOUR_H_ */