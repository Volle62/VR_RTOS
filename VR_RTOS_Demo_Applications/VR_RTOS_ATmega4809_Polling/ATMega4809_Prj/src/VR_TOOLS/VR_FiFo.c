// Volker Rumm, 02.06.2024                                                                         *


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "VR_TOOLS/VR_FiFo.h"

void VR_InitFiFo(VR_FiFo_t *pFifo, uint8_t *Buff, uint16_t Size)
{
  pFifo->BufferSize = Size;
  pFifo->Buffer = Buff;
  pFifo->IndexWr = 0;
  pFifo->IndexRd = 0;
  pFifo->BufferCount = 0;
}

bool VR_PutFiFo(VR_FiFo_t *pFiFo, uint8_t data)
{
  if (pFiFo->BufferCount == pFiFo->BufferSize)
  {
    // FiFo ist voll !!!
    return (false);
  }

  pFiFo->Buffer[pFiFo->IndexWr] = data;
  pFiFo->IndexWr++;
  if (pFiFo->IndexWr >= pFiFo->BufferSize)
    pFiFo->IndexWr = 0;
  pFiFo->BufferCount++;

  return (true);
}

bool VR_PutFiFo_Size(VR_FiFo_t *pFiFo, uint8_t *pData, uint16_t dataSize)
{
  if ((pFiFo->BufferCount + dataSize) >= pFiFo->BufferSize)
  {
    // FiFo ist voll !!!
    return (false);
  }

  while (dataSize)
  {
    pFiFo->Buffer[pFiFo->IndexWr] = *pData;
    pFiFo->IndexWr++;
    if (pFiFo->IndexWr >= pFiFo->BufferSize)
            pFiFo->IndexWr = 0;
    dataSize--;
    pData++;
    pFiFo->BufferCount++;
  }
	
  return (true);
}

bool VR_GetFiFo(VR_FiFo_t *pFiFo, uint8_t *pData)
{
  if (pFiFo->BufferCount == 0)
  {
    // FiFo ist leer !!!
    return (false);
  }

  // Inkrement; automatischer �berlauf bei 256 !!!
  *pData = pFiFo->Buffer[pFiFo->IndexRd];
  pFiFo->IndexRd++;
  if (pFiFo->IndexRd >= pFiFo->BufferSize)
    pFiFo->IndexRd = 0;
  pFiFo->BufferCount--;
  
  return (true);
}

uint16_t VR_GetFiFo_Count(VR_FiFo_t *pFiFo)
{
  return (pFiFo->BufferCount);
}

bool VR_Is_FiFo_Full(VR_FiFo_t *pFiFo)
{
  return (pFiFo->BufferCount == pFiFo->BufferSize);
}

bool VR_Is_FiFo_Empty(VR_FiFo_t *pFiFo)
{
  return (pFiFo->BufferCount == 0);
}
