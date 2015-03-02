#include "pack.h"

#include "search.c"

DATATYPE unsigned char * array;
DATATYPE uint32_t * busylist;
DATATYPE unsigned long long  changed[CACHESIZE];

#ifdef DEBUG
static uint32_t f1,f2,f3;
static int option_v;

//#define F1 0x88000000
#define F1 0x80000001
#define F2 1
#define F3 3
#define pprintf(args...) do{if((f1==F1)&&(f2==F2)&&(f3==F3)) {printf("%d:",__LINE__);printf(args);};}while(0)
#else
#define pprintf(args...) do{}while(0)
#endif


PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
        uint32_t busy,iwhite,idamka;
	uint32_t idx;
        Pack(&busy,&iwhite,&idamka,w,b,d);
	idx = search(busy,busylist,CNK);
        switch(twobit_get(array + abytes(RANK,idx),(iwhite<<RANK)|idamka)){
        case 3 : // Cimus ZZ
		return 0;
        case 0 :
		return 0;
        case 1 :
		return 5;
        case 2 :
		return -5;
        default:
#ifdef DEBUG
		error("%08X %X %X -> %08X %08X %08X",f1,f2,f3,busy,iwhite,idamka);
#endif
		return 0;
        }
}
PROCTYPE inline int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
        return StaticWhite(_brev(w),_brev(b),_brev(d));
}

#include "move4.c"

KERNEL
	unsigned ij;
	unsigned char * job  = array + abytes(RANK,idx);
	uint32_t busy = busylist[idx];

	for(ij=0; ij< (1<<2*RANK); ij++) {
#ifdef WRANK
	int wrank = _popc(ij>>RANK);
	if( (wrank!=WRANK) && (wrank!= (RANK-WRANK))) continue;
#endif
	if(twobit_get(job,ij)==0) {
	        uint32_t w,b,d;
   		int r;
#ifdef DEBUG
        	f1=busy,f2=ij>>RANK,f3=ij&RMASK;
#endif
        	Unpack(busy,ij>>RANK,ij&RMASK,&w,&b,&d);
       		r = MoveWhite(w,b,d);
		if(r==0) continue;
#ifdef DEBUG
		if(option_v)
			printf("%08X %X %X %d\n",busy,i,j,r);
#endif
		twobit_set(job,ij,r<0?2:1);
		atomicAdd(changed+idx%CACHESIZE,1);
	}
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
