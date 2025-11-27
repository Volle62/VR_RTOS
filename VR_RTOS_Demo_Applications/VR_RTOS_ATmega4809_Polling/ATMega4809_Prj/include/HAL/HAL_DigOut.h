// HAL_DigOut.h

#ifndef HAL_DIGOUT_H_
#define HAL_DIGOUT_H_

#include "HAL/HAL_Config/HAL_DigOut_Cfg.h"

void HAL_Init_DigOuts(void);
void HAL_Set_DigOut(uint8_t digOut_no);
void HAL_Reset_DigOut(uint8_t digOut_no);
void HAL_SetRes_DigOut(uint8_t digOut_no, bool state);
void HAL_Toggle_DigOut(uint8_t digOut_no);
bool HAL_Get_DigOut(uint8_t digOut_no);
char *HAL_Get_DigOut_Name(uint8_t digOut_no);
uint8_t HAL_Get_DigOut_No(char *DigOut_Name);
uint8_t HAL_get_MaxDigOut();

typedef struct
{
	char   *DigOut_Name;
	uint8_t Port; // for example 'B'
	uint8_t Pin;  // for example 5
	bool    invert; // is the output inverted
} HAL_DigOut_t;


#endif /* HAL_DIGOUT_H_ */