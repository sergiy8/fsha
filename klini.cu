
DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];

#if NODAMKA
#undef IN_klini
#include "megask.c"
#define IN_klini 1
#include <pthread.h>
static pthread_key_t key;
static int question(TPACK x) {
	FILE * f = pthread_getspecific(key);
	if (fwrite(&x,sizeof(x),1,f) != 1)
		error("%m");
	return R_UNK;
}
typedef void (*DESTRUCTOR)(void *);
#define EXTRA_INIT     megask_init(); pthread_key_create(&key, (DESTRUCTOR)fclose);
#endif

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
		TPACK x = TPack((T12){w,b,d});
#if NODAMKA
	switch(megask(x)) {
		case ASK_NODB:
			if(d)
				return question(x);
		case ASK_UNK:
			return R_UNK;
		case ASK_DRAW:
			return R_DRA;
		case ASK_WHITE:
			return R_WIN;
		case ASK_BLACK:
			return R_LOS;
		default:
			error("Smth wrong: %08X %X %X = %d", x.b, x.w, x.d, megask(x));
	}
#else
		uint32_t idx = blist_get(x.b);
        switch(twobit_get(array + (uint64_t)((x.w<<RANK)|x.d) * JOB_SIZE, idx)){
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
#endif
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
		case R_UNK:
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
