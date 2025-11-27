// HW_DigIn.h

#ifndef HW_DIGIN_H_
#define HW_DIGIN_H_

void HW_Init_DigIn(uint8_t port, uint8_t pin);
bool HW_Get_DigIn(uint8_t port, uint8_t pin);
uint16_t getNumberOfDigIns(void);

#endif /* HW_DIGIN_H_ */