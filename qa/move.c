#include "sha.h"
#include "getarg.h"

static int MoveBlack(T12 arg){
	TPACK pos = TPack(arg);
	printf(" %08X %X %X\n",pos.b, pos.w, pos.d);
	return 0;
}

#define MOVE_FIND_ALL 1
#include "move4.c"


int main(int argc, char ** argv){
	TPACK pos = { getarg(1), getarg(2), getarg(3) };
	printf("%08X %X %X\n",pos.b, pos.w, pos.d);
	MoveWhite(TUnpack(pos));
	return 0;
}
