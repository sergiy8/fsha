
#include "sha.h"
static int rank;
#define RANK rank
#include "facecontrol.h"
#include "getarg.h"

int main(int argc, char ** argv){
	uint32_t b,w,d;
	if(argc!=4) panic();
	b = getarg(1);
	w = getarg(2);
	d = getarg(3);
	rank = __builtin_popcount(b);
	printf("%d %08X %X %X\n",FaceControl(b,w,d),b,w,d);
	return 0;
}
