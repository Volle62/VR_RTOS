// HW_AnaIn.h

#ifndef HW_ANAIN_H_
#define HW_ANAIN_H_

void HW_Init_AnaIn(uint8_t Ch);
void HW_Start_AnaIn(uint8_t Ch);
bool HW_Is_AnaIn_Ready(void); // is the ADC conversion done ?
uint16_t HW_Read_AnaIn(void);

#endif /* HW_ANAIN_H_ */