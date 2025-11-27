// HAL_DigIn.h

#ifndef HAL_DIGIN_H_
#define HAL_DIGIN_H_

#include "HAL/HAl_Config/HAL_DigIn_Cfg.h"

void HAL_Init_DigIns(void);
bool HAL_Get_DigIn(uint8_t digIn_no);
char *HAL_Get_DigIn_Name(uint8_t digIn_no);
uint8_t HAL_Get_DigIn_No(char *DigIn_Name);
uint8_t HAL_get_MaxDigIn();

typedef struct
{
	char   *DigIn_Name;
	uint8_t Port; // for example 'B'
	uint8_t Pin;  // for example 5
	bool    invert; // is the input inverted
    bool    value;  // DigIn Value
} HAL_DigIn_t;


#endif /* DIGIN_H_ */