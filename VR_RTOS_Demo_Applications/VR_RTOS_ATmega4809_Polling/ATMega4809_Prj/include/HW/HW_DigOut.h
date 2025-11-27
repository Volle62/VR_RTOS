// HW_DigOut.h

#ifndef HW_DIGOUT_H_
#define HW_DIGOUT_H_

void HW_Init_DigOut(uint8_t port, uint8_t pin);
void HW_SetRes_DigOut(uint8_t port, uint8_t pin, bool state);
void HW_Toggle_DigOut(uint8_t port, uint8_t pin);
bool HW_Get_DigOut(uint8_t port, uint8_t pin);
uint16_t getNumberOfDigOuts(void);

#endif /* HW_DIGOUT_H_ */