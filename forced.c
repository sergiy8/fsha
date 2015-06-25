#include "sha.h"
PROCTYPE inline int MoveBlack(T12 pos){
	return R_LOS; // stop futher search
}
#define IN_before 1
#define MoveWhite MoveWhiteTake
#include "move5.c"

#undef IN_before
#undef MoveWhite
#define IN_klini 1
#define MoveWhite MoveWhiteRaw
#include "move5.c"

int IsForced(TPACK cp){
	T12 pos = TUnpack(cp);
	if (MoveWhiteTake(pos) != R_NOM)
			return -1;
	if (MoveWhiteRaw(pos) != R_NOM)
			return 0;
	return -2;
}
