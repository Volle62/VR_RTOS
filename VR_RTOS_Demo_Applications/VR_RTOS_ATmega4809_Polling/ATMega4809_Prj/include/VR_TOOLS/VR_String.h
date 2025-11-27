// Volker Rumm, 02.06.2024                                                                         *


#include <stdlib.h>
#include <stdarg.h>

// copy first string to destination and concat all following strings
// to this until the NULL-Paparameter occur
// example : Tools_StrCpy(dest, "aaa", "bbb", "ccc", NULL);
// result sting dest : "aaabbbccc"
extern char *VR_StrCpy(char *Dst, ...);

// concat all strings to destination
// to this until the NULL-Paparameter occur
// example : dest contents the string "123"; 
//  Tools_StrCpy(dest, "aaa", "bbb", "ccc", NULL);
// result sting dest : "123aaabbbccc"
extern char *VR_StrCat(char *Dst, ...);

//convert the number to Strg, Base is 10, without leading '0'
extern char *VR_Itoa(int32_t Number, char *Strg);

//convert string to number, Base is 10
extern int32_t VR_Atoi(char *Strg);

//get the nibbles of a number
extern uint8_t VR_GetHighNibble(uint8_t Byte);
extern uint8_t VR_GetLowNibble(uint8_t Byte);
