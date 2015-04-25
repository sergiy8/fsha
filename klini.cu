#include "pack.h"

DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];


PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
        uint32_t busy,iwhite,idamka;
	uint32_t idx;
        Pack(&busy,&iwhite,&idamka,w,b,d);
		idx = blist_get(busy);
        switch(twobit_get(array + (uint64_t)((iwhite<<RANK)|idamka) * JOB_SIZE, idx)){
        case 3 : // Cimus ZZ
		return 0;
        case 0 :
		return 0;
        case 1 :
		return 5;
        case 2 :
		return -5;
        default:
		return 0;
        }
}
PROCTYPE inline int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
        return StaticWhite(_brev(w),_brev(b),_brev(d));
}

static unsigned spewcount[CACHESIZE];
#define SPEW_LEVEL 128

#include "move4.c"
KERNEL
    unsigned i = ij >> RANK;
    unsigned j = ij & RMASK;
    unsigned busy;
    unsigned idx;
    for(idx=0,busy=ALLONE(RANK);_popc(busy)==RANK;idx++,busy = _permut(busy))
	if(twobit_get(job,idx)==0) {
	        uint32_t w,b,d;
   		int r;
        	Unpack(busy,i,j,&w,&b,&d);
       		r = MoveWhite(w,b,d);
		if(r==0) continue;
		twobit_set(job,idx,r<0?2:1);
		if ( spewcount[ij%CACHESIZE]++ % SPEW_LEVEL == 0 )
			fprintf(stderr,"%08X %X %x %d\n",busy,i,j,r<0?2:1);
		atomicAdd(changed+ij%CACHESIZE,1);
	}
}

static void PutStat(void){
	FILE * f = fopen(STATFILE(RANK),"w");
	if(f==NULL) return;
#ifdef WRANK
	fprintf(f,"klini WRANK=%d\n",WRANK);
#else
	fprintf(f,"klini\n");
#endif
	fclose(f);
}
