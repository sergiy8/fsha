#include "sha.h"
#include "pack.h"
#include "getarg.h"

static int MoveBlack(uint32_t mine, uint32_t his, uint32_t d){
	uint32_t busy,iw,id;
	Pack(&busy,&iw,&id,his,mine,d);
	printf(" %08X %X %X (%08X %08X %08X)\n",busy,iw,id,his,mine,d);
	return 0;
}

#define mprintf printf
#define MOVE_FIND_ALL 1
#include "move4.c"


int main(int argc, char ** argv){
	uint32_t busy,iw,id;
	uint32_t w,b,d;
	busy = getarg(1);
	iw = getarg(2);
	id = getarg(3);
	Unpack(busy,iw,id,&w,&b,&d);
	printf("%08X %X %X  (%08X %08X %08X)\n",busy,iw,id,w,b,d);
	MoveWhite(w,b,d);
	return 0;
}
