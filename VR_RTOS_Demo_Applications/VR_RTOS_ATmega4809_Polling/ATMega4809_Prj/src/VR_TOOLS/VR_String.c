// Volker Rumm, 02.06.2024      


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdbool.h>

#include "VR_TOOLS/VR_String.h"

char* VR_StrCpy(char* pDst, ...)
{
  va_list argv;
  char* pSrc;
  char* pStrg;

  // if pDst invalid, return NULL
  if (!pDst)
    return (NULL);

  va_start(argv, pDst);

  pStrg = pDst;

  do
  {
    pSrc = va_arg(argv, char*);
    if (pSrc)
    {
      // append dest to src
      while (*pSrc)
      {
        *pDst = *pSrc;
        pDst++;
        pSrc++;
      }
    }
  } while (pSrc);

  va_end(argv);

  *pDst = 0; // end of string

  return (pStrg);
}

char* VR_StrCat(char* pDst, ...)
{
  va_list argv;
  char* pSrc;
  char* pStrg;

  // if pDst invalid, return NULL
  if (!pDst)
    return (NULL);

  va_start(argv, pDst);

  pStrg = pDst;

  // move pDst to end of string
  while (*pDst)
    pDst++;

  do
  {
    pSrc = va_arg(argv, char*);
    if (pSrc)
    {
      // append dest to src
      while (*pSrc)
      {
        *pDst = *pSrc;
        pDst++;
          pSrc++;
      }
    }
  } while (pSrc);

  va_end(argv);

  *pDst = 0; // end of string

  return (pStrg);
}

char *VR_Itoa(int32_t Number, char *Strg)
{
  uint8_t digit;
  uint8_t digit_no=0;
  int8_t i, j;
  char help;
  bool negative=false;
  
  if (Number == 0)
  {
    Strg[0] = '0';
    Strg[1] = 0;
    return (Strg);
  }

  if (Number < 0)
  {
    Number *=-1;
    negative = true;
    Strg[0] = '-';
    digit_no = 1;
  }
  
  while (Number > 0)
  {
    digit = Number %  10;
    Strg[digit_no] = '0' + digit;
    Number /= 10;
    digit_no++;
  }
  Strg[digit_no] = 0; // end of string
  
  // string umdrehen
  if (negative)
    i = 1;
  else
    i = 0;
  j = digit_no - 1;
  while (i < j)
  {
    help = Strg[i];
    Strg[i] = Strg[j];
    Strg[j] = help;
    i++;
    j--;
  }
  
  return (Strg);
}

int32_t VR_Atoi(char *Strg)
{
  int32_t val=0;
  int32_t fakt=1;
  
  if (!Strg)
    return (0);
  
  if (!(*Strg))
    return (0);

  if (*Strg == '-')
  {
    fakt = -1;
    Strg++;
  }
  
  while ((*Strg >= '0') && (*Strg <= '9'))
  {
    val *= 10;
    val += (*Strg - '0');
    Strg++;
  }
  
  return (fakt * val);
}

uint8_t VR_GetHighNibble(uint8_t Byte)
{
  uint8_t cc = ((Byte >> 4) & 0x0F);
  
  if (cc < 10)
    return ('0' + cc);
  else
    return ('A' + cc - 10);
}

uint8_t VR_GetLowNibble(uint8_t Byte)
{
  uint8_t cc = (Byte & 0x0F);
  
  if (cc < 10)
    return ('0' + cc);
  else
    return ('A' + cc - 10);
}
