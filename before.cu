#include "pack.h"
#include "permut.h"
#include "ask.c"

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
        uint32_t busy,iwhite,idamka;
        Pack(&busy,&iwhite,&idamka,w,b,d);
        switch(Ask(busy,iwhite,idamka)) {  // RANK of ask is always less the our
		case 3:
        case 0 : return 0;
        case 1 : return 5;
        case 2 : return -5;
	default:
			return 0;
        }
}
PROCTYPE inline int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
        return StaticWhite(_brev(w),_brev(b),_brev(d));
}

#include "move4.c"


DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];

KERNEL
    unsigned i = ij >> RANK;
    unsigned j = ij & RMASK;
    unsigned busy;
    unsigned idx;
    unsigned char * job = array + ij * CNK /4;
    for(idx=0,busy=ALLONE(RANK);_popc(busy)==RANK;idx++,busy = _permut(busy))
	if(twobit_get(job,idx)==0) {
	        uint32_t w,b,d;
   			int r;
        	Unpack(busy,i,j,&w,&b,&d);
       		r = MoveWhite(w,b,d);
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

