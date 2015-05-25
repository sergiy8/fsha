#include "permut.h"
#include "malloc_file.c"


DATATYPE unsigned char * known[9];

PROCTYPE int Ask(TPACK pos){
    int arank = _popc(pos.b);
//  if(arank==0 || arank>8) error("Error ask");
    uint32_t  idx  = blist_get(pos.b);
    return twobit_get(known[arank] + (uint64_t)((pos.w<<arank) | pos.d) * cnk(32,arank)/4, idx);
}

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
        switch(Ask(TPack((T12){w,b,d}))) {  // RANK of ask is always less the our
		case 3:
        case 0 : return R_DRA;
        case 1 : return R_WIN;
        case 2 : return R_LOS;
	default:
			return 0;
        }
}
PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}


#include "move5.c"

DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];

KERNEL
#if NODAMKA
    unsigned i = ij;
    const unsigned j = 0;
#else
    unsigned i = ij >> RANK;
    unsigned j = ij & RMASK;
#endif
    unsigned busy;
    unsigned idx;
    for(idx=0,busy=ALLONE(RANK);_popc(busy)==RANK;idx++,busy = _permut(busy))
	if(twobit_get(job,idx)==0) {
   			int r;
			r = MoveWhite(TUnpack((TPACK){busy,i,j}));
			switch(r) {
			case R_NOM:
        			continue;  // rmain unknown
			case R_WIN:
					r=1;
					break;
			case R_LOS:
					r=2;
					break;
			case R_DRA:
        			r=3; // can take - but for draw position
					break;
			default:
					error("Smth wrong, r=%d",r);
			}
		twobit_set(job,idx,r);
		atomicAdd(changed+ij%CACHESIZE,1);
	}
}
