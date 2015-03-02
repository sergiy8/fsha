#include "sha.h"
#include "getarg.h"
//#define haszero(v) !!((( v ) - 0x01010101UL) & ~( v ) & 0x80808080UL)
#define haszero(v) !!((( v ) - 0x55) & ~( v ) & 0xAA)

static int checkzero(unsigned i) {
	if((i&0x03)==0) return 1;
	if((i&0x0c)==0) return 1;
	if((i&0x30)==0) return 1;
	if((i&0xc0)==0) return 1;
	return 0;
}

int main(int argc, char ** argv){
#if 0
	uint32_t x = getarg(1);
	printf("%08X %d\n",x,haszero(x));
#else
	unsigned i;
	for(i=0;i<256;i++)
		if(haszero(i) != checkzero(i)) printf("%d %d %d\n",
			i,haszero(i),checkzero(i));
#endif
	return 0;
}
