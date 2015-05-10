#include "sha.h"
PROCTYPE inline int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
	return 0;
}
#define IN_before 1
#include "move4.c"
int IsForced(TPACK cp){
	uint32_t x,y,z;
	TUnpack(cp,&x,&y,&z);
	return MoveWhite(x,y,z) != R_NOMOVE;
	return -1; // Negative - stop futher search
}
