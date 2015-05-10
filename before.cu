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
        case 0 : return 0;
        case 1 : return 5;
        case 2 : return -5;
	default:
			return 0;
        }
}
PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

#include "move4.c"


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
        	if(r==R_NOMOVE) continue;  // rmain unknown
        	if(r<0) r=2;
        	else if(r>0) r=1;
        	else r=3; // can take - but for draw position
		twobit_set(job,idx,r);
		atomicAdd(changed+ij%CACHESIZE,1);
	}
}

static void PutStat(void){
        FILE * f = fopen(STATFILE(RANK),"w");
        if(f==NULL) return;
        fprintf(f,"before\n");
        fclose(f);
}

