// Volker Rumm, 02.06.2024                                                                         *


#ifndef VR_FIFO_H
#define VR_FIFO_H

typedef struct
{
	uint16_t BufferSize;
	uint8_t *Buffer; 
	uint16_t IndexWr;
	uint16_t IndexRd;
	uint16_t BufferCount;
} VR_FiFo_t;


void VR_InitFiFo(VR_FiFo_t *pFifo, uint8_t *Buff, uint16_t Size);
bool VR_PutFiFo(VR_FiFo_t *pFiFo, uint8_t data);
bool VR_PutFiFo_Size(VR_FiFo_t *pFiFo, uint8_t *pData, uint16_t dataSize);
bool VR_GetFiFo(VR_FiFo_t *pFiFo, uint8_t *pData);
uint16_t VR_GetFiFo_Count(VR_FiFo_t *pFiFo);
bool VR_Is_FiFo_Full(VR_FiFo_t *pFiFo);
bool VR_Is_FiFo_Empty(VR_FiFo_t *pFiFo);

#endif //#define VR_FIFO_H