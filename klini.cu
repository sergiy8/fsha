
DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];


PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
		TPACK x = TPack((T12){w,b,d});
	uint32_t idx;
#if NODAMKA
	if(idamka){
		printf("%08X %X %X\n",x.b,x.w,x.d);
		return 0;
	}
#endif
		idx = blist_get(x.b);
#if NODAMKA
        switch(twobit_get(array + (uint64_t)iwhite * JOB_SIZE, idx)){
#else
        switch(twobit_get(array + (uint64_t)((x.w<<RANK)|x.d) * JOB_SIZE, idx)){
#endif
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
PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

static unsigned spewcount[CACHESIZE];
#define SPEW_LEVEL (1<<0)

#include "move4.c"
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
