#include "pack.h"
#include "permut.h"
#include "ask.c"

#ifdef DEBUG
static uint32_t fbusy,fij;
static int option_v;
#endif

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
        uint32_t busy,iwhite,idamka;
        Pack(&busy,&iwhite,&idamka,w,b,d);
        switch(Ask(busy,iwhite,idamka)) {  // RANK of ask is always less the our
		case 3:
        case 0 : return 0;
        case 1 : return 5;
        case 2 : return -5;
	default:
#ifdef DEBUG
            error("%08X %X %X -> %08X %X %X",fbusy,fij>>RANK,fij&RMASK,busy,iwhite,idamka);
#endif
	return 0;
        }
}
PROCTYPE inline int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
        return StaticWhite(_brev(w),_brev(b),_brev(d));
}

#include "move4.c"


DATATYPE unsigned char * array;
DATATYPE uint32_t * busylist;
DATATYPE unsigned long long  changed[CACHESIZE];

KERNEL
	unsigned ij;
	unsigned char * job  = array + abytes(RANK,idx);
	uint32_t busy = busylist[idx];

#ifdef REVERSE
	for(ij=(1<<(2*RANK))-1; ij; ij--){
#else
	for(ij=0;ij< (1<<(2*RANK)); ij++){
#endif
	if(twobit_get(job,ij)==0) {
	        uint32_t w,b,d;
   		int r;
        	Unpack(busy,ij>>RANK,ij&RMASK,&w,&b,&d);
#ifdef DEBUG
        	fbusy=busy, fij = ij;
#endif
       		r = MoveWhite(w,b,d);
        	if(r==R_NOMOVE) continue;  // rmain unknown
        	if(r<0) r=2;
        	else if(r>0) r=1;
        	else r=3; // can take - but for draw position
#ifdef DEBUG
		if( (r!=3)&&option_v)
			printf("%08X %08X %08X %d\n",w,b,d,r);
#endif
		twobit_set(job,ij,r);
		atomicAdd(changed+idx%CACHESIZE,1);
	}
	}
#if RANK >=7 // abytes(RANK,1) >= PAGE_SIZE
	munmap(job,abytes(RANK,1));
#endif
}

static void PutStat(void){
        FILE * f = fopen(STATFILE(RANK),"w");
        if(f==NULL) return;
        fprintf(f,"before\n");
        fclose(f);
}

