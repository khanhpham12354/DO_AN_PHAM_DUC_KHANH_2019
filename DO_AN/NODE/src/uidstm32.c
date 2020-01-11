#include "uidstm32.h"

unsigned int hex_to_int(char x){
	unsigned int kq;
	switch(x){
		case 'A':
			kq = 10;
			break;
		case 'B':
			kq = 11;
			break;
		case 'C':
			kq = 12;
			break;
		case 'D':
			kq = 13;
			break;
		case 'E':
			kq = 14;
			break;
		case 'F':
			kq = 15;
			break;
		default:
			kq = x-'0';
	}
	return kq;
}

unsigned int hex_to_ascii(char x,char y){
	unsigned int chuc,donvi;
	chuc = hex_to_int(x);
	donvi = hex_to_int(y);
	return (chuc*16 + donvi);
}

void hexStr_to_asciiStr(char hexStr[],char Str[]){
	unsigned int i = 0,j=0;
	for(i = 0 ;i < strlen(hexStr);i=i+2){
		Str[j] = hex_to_ascii(hexStr[i],hexStr[i+1]);
		j++;
	}
	Str[j] = '\0';
}

void get_UID(char UID_Str[]){
	char buffer[10];
	uint32_t idPart1 = STM32_UID[0];
	uint32_t idPart2 = STM32_UID[1];
	uint32_t idPart3 = STM32_UID[2];
	sprintf(buffer,"%X%X",idPart1&0x00FFFFFF,idPart2);
	hexStr_to_asciiStr(buffer,UID_Str);
}
