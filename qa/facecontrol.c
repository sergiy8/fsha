
#include "sha.h"
#include "facecontrol.h"
#include "getarg.h"

int main(int argc, char ** argv){
	TPACK pos = { getarg(1), getarg(2), getarg(3) };
	printf("%d %08X %X %X\n",FaceControl(pos),pos.b,pos.w,pos.d);
	return 0;
}
