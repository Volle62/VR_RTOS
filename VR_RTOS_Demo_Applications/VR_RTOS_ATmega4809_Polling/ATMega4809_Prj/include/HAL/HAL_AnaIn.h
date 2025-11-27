// HAL_AnaIn.h

#ifndef HAL_ANAIN_H_
#define HAL_ANAIN_H_

#include "HAL/HAL_Config/HAL_AnaIn_Cfg.h"

void HAL_Init_AnaIns(void);
void HAL_Start_AnaIn(uint8_t Ch);
bool HAL_Is_AnaIn_Ready(void); // is the ADC conversion done ?
uint16_t HAL_Read_AnaIn(void);
uint16_t HAL_Get_AnaIn_Value(uint8_t AnaIn_no);
char *HAL_Get_AnaIn_Name(uint8_t AnaIn_no);
uint8_t HAL_Get_AnaIn_No(char *AnaIn_Name);
uint8_t HAL_get_MaxAnaIn(void);
uint16_t getNumberOfAnaIns(void);

typedef struct
{
	char   *AnaIn_Name;
	uint8_t Channel; // 0 .. 15
    uint8_t value;  // DigIn Value
} HAL_AnaIn_t;


#endif /* ANAIN_H_ */