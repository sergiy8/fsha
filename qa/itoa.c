#include "sha.h"

#include <itoa/itoa.h>

#define IS_BIG_ENDIAN() (!(union { uint16_t u16; unsigned char c; }){ .u16 = 1 }.c)

int main(int argc, char ** argv){
	uintmax_t x;
	printf("IS_BIG_ENDIAN:%d\n",IS_BIG_ENDIAN());
	if(argc!=2) error("./itoa <x>");
	x= strtoull(argv[1],NULL,0); 
	printf("%jx %jd %s\n",x,x,itoa(x));
	return 0;
}
