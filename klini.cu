
DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];

#if KLINI_MEGASK
#include "megask.c"
#endif

#if NODAMKA
static FILE * pf; // Text with unknown combinations
static int option_q;
void pf_init(void){
	char fname[PATH_MAX];
	snprintf(fname,sizeof(fname),DATADIR "%d-q%d.unsorted", RANK, option_q);
	pf = fopen(fname,"w");
	if( pf == NULL)
		error ("Cannot fopen %s",fname);
}
#endif


PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
		TPACK x = TPack((T12){w,b,d});
#if KLINI_MEGASK
	switch(megask(x)) {
		case ASK_DRAW:
		case ASK_NODB:
		case 3:
			return 0;
		case ASK_WHITE:
			return 5;
		case ASK_BLACK:
			return -5;
		default:
			error("Smth wrong: %08X %X %X = %d", x.b, x.w, x.d, megask(x));
	}
#else
#if NODAMKA
	if(x.d)
	if(option_q){
		if( fwrite(&x,sizeof(x),1,pf) != 1)
			error("fwrite()");
		return 0;
	}
#endif
		uint32_t idx = blist_get(x.b);
#if NODAMKA
        switch(twobit_get(array + (uint64_t)x.w * JOB_SIZE, idx)){
#else
        switch(twobit_get(array + (uint64_t)((x.w<<RANK)|x.d) * JOB_SIZE, idx)){
#endif
        case 3 : // Cimus ZZ
		return R_DRA;
        case 0 :
		return R_DRA;
        case 1 :
		return R_WIN;
        case 2 :
		return R_LOS;
        default:
			error("Smth wrong");
        }
#endif // KLINI_MEGASK
}
PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

static unsigned spewcount[CACHESIZE];
#define SPEW_LEVEL (1<<20)

#include "move5.c"
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
		case R_DRA:
			continue;
		case R_WIN:
			twobit_set(job,idx,1);
			break;
		case R_NOM:
		case R_LOS:
			twobit_set(job,idx,2);
			break;
		default:
			error("Smth wrong, r=%d",r);
		}
		if ( spewcount[ij%CACHESIZE]++ % SPEW_LEVEL == 0 )
			fprintf(stderr,"%08X %X %x %d\n",busy,i,j,r==R_WIN?1:2);
		atomicAdd(changed+ij%CACHESIZE,1);
	}
}
