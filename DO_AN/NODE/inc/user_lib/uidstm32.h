#ifndef _UIDSTM32_H_
#define _UIDSTM32_H_

#include "stm32l1xx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*define address of UID resgister(96 bits)*/
#define 				STM32_UID 		 ((uint32_t *)0x1FF80050)

unsigned int hex_to_int(char x);
unsigned int hex_to_ascii(char x,char y);
void hexStr_to_asciiStr(char hexStr[],char Str[]);
void get_UID(char UID[]);


#endif
