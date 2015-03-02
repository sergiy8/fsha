#include "sha.h"
#include "getarg.h"
static int b,w,d;
static char pole[32];
int main(int argc, char ** argv){
	int i,j;
	b = getarg(1);
	w = getarg(2);
	d = getarg(3);
	printf("%08X %X %X\n",b,w,d);
	while(b) {
		int i = _ffs(b)-1;
		pole[i] = w&1? 'w' : 'b';
		if(d&1) pole[i] ^= 0x20;
//printf("%d %c\n",i,pole[i]);
		w/=2;
		d/=2;
		b&=b-1;
	}
	for(i=0;i<8;i++) {
		if((i&1)==0) printf("  ");
		for(j=0;j<4;j++){
		   int k = (28-4*i) + j;
		   printf(" %c ",pole[k]?pole[k]:'.');
		}
		printf("\n");
	}
	return 0;
}
