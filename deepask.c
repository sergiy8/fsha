static ask_t megask(TPACK pos);

PROCTYPE int DeepWhite(uint32_t w, uint32_t b, uint32_t d){
        switch(megask(TPack((T12){w,b,d}))) {  // RANK of ask is always less the our
		case ASK_NODB:
				return R_UNK;
		case ASK_WHITE:
				return R_WIN;
		case ASK_BLACK:
				return R_LOS;
		case ASK_DRAW:
				return R_DRA;
		case ASK_UNK:
			error("Smth wromg: megask should not return ASK_UNK after takege!");
		default:
			error("Smth is wrong");
        }
}


PROCTYPE inline int MoveBlackDeepAsk(T12 pos){
	return DeepWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

#define IN_before 1
#define MoveWhite MoveWhiteDeepAsk
#define MoveBlack MoveBlackDeepAsk
#define Chain ChainDeepAsk
#include "move5.c"
#undef MoveWhite
#undef MoveBlack
#undef Chain
#undef IN_before
#undef MOVE_FIND_ALL


int deepask(TPACK x) {
	T12 pos = TUnpack(x);
	int r = MoveWhiteDeepAsk(pos);
	switch(r) {
		case R_NOM:
			return ASK_NODB;
		case R_LOS:
			return ASK_BLACK;
		case R_WIN:
			return ASK_WHITE;
		case R_UNK:
			return ASK_NODB;
		case R_DRA:
			return ASK_DRAW;
		default:
			error("Smth is wrong");
	}
}
