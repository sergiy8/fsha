#include "sha.h"
PROCTYPE inline int MoveBlack(T12 pos){
	return -1; // Negative - stop futher search
}
#define IN_before 1
#define MoveWhite MoveWhiteTake
#include "move4.c"

#undef IN_before
#undef MoveWhite
#define IN_klini 1
#define MoveWhite MoveWhiteRaw
#include "move4.c"

int IsForced(TPACK cp){
	T12 pos = TUnpack(cp);
	if (MoveWhiteTake(pos) != R_NOMOVE )
			return -1;
	if (MoveWhiteRaw(pos) != R_NOMOVE )
			return 0;
	return -2;
}
