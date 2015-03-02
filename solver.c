#define IN_solver
//#define USE_DBASK 1
#include <time.h>
#include <pthread.h>
#include "sha.h"
#include "getarg.h"
#include "pack.h"
static time_t started;

typedef uint32_t db_t;
struct ITEM {
db_t left, right;
uint32_t db;
unsigned dw:24,dd:24;
int dr:8;
} __attribute__((packed));
static struct ITEM * dbase;
static int db_compar(db_t  a, db_t b){
        struct ITEM *xa = dbase + a;
        struct ITEM *xb = dbase + b;
        if (xa->db < xb->db) return -1;
        if (xa->db > xb->db) return  1;
        if (xa->dw < xb->dw) return -1;
        if (xa->dw > xb->dw) return  1;
        if (xa->dd < xb->dd) return -1;
        if (xa->dd > xb->dd) return  1;
        return 0;
}
#define DBASE_FNAME "dbase_solver"
#define DBSIZE ((db_t)(1<<31))
#include "db.c"

static db_t rootp;
static db_t lastp;


#ifdef USE_DBASK
#include "dbutil/dbask.c"
#else
#include "ask.c"
#endif

static int level;
static int option_e = RANK - 1;
static int frank ; // first rank

static enum {EXPAND,KLINI,SHOW} mode;

int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
	int r;
	uint32_t busy,iwhite,idamka;
        Pack(&busy,&iwhite,&idamka,_brev(w),_brev(b),_brev(d));
#if USE_DBASK
	if(dbknown(_popc(busy))) {
		r = dbask(busy,iwhite,idamka);
#else
	if(known[_popc(busy)]) {
		r = Ask(busy,iwhite,idamka);
#endif
		switch(r) {
		case 3 : // error("Ill!");
		case 0 : return 0;
		case 1 : r =  127 - level - _popc(b); goto s_return;
		case 2 : r = -127 + level + _popc(w); goto s_return;
		}
	}
	db_t x = db_alloc();
	dbase[x].db = busy;
	dbase[x].dw = iwhite;
	dbase[x].dd = idamka;
	dbase[x].dr = 0;
	db_t res = tsearch(x,&rootp);
	if(res==x) // new in table
		return 0;
	db_free();
	r = dbase[res].dr;
	if(r>0) r--;
	if(r<0) r++;
s_return:
	if( mode == SHOW)
	   {
        	Pack(&busy,&iwhite,&idamka,b,w,d);
			if(r==0)
				printf("%08X %x %X =%d\n",busy,iwhite,idamka,-r);
			else {
				uint32_t b2,w2,d2;
				Pack(&b2,&w2,&d2,_brev(w),_brev(b),_brev(d));
				printf("%08X %x %X =%d WIN %X %x %x\n",busy,iwhite,idamka,-r,b2,w2,d2);
			}
	   }
	return r;
}

#define MOVE_FIND_ALL (level==0)
#include "move4.c"

static uintmax_t changed[NPROC];
static pthread_t pid[NPROC];
static void * thread(void * arg){
	db_t i ;
	uint32_t w,b,d;
	int r;
	for(i=(uintptr_t)arg;i<lastp;i+=NPROC) {
		if(dbase[i].dr) continue;
		Unpack(dbase[i].db,dbase[i].dw,dbase[i].dd,&w,&b,&d);
		r  = MoveWhite(w,b,d);
		if (r) {
			dbase[i].dr = r;
			changed[((uintptr_t)arg)%NPROC]++;
		}
	}
	return NULL;
}

static void usage(void){
	error("\n\tUsage: ./solver [-e endspile-use] BUSY IWHITE IDAMKA");
}
int main(int argc, char ** argv){
	db_t i;
	uint32_t w,b,d;
	db_t first;
        while ((i = getopt(argc, argv, "e:")) != -1)
        switch (i) {
	default: usage();
	case 'e': option_e = atoi(optarg); continue;
	}
dbase = malloc_file(sizeof(struct ITEM)*DBSIZE,FMODE_CR,DBASE_FNAME);

	lastp = dbsize;
        first = db_alloc();
	rootp = first;

        dbase[first].db = getarg(optind);
        dbase[first].dw = getarg(optind+1);
        dbase[first].dd = getarg(optind+2);
        dbase[first].dr = 0;

	frank = _popc(dbase[first].db);

	if(option_e > frank) option_e = frank;
printf("Endspiles upto rank:%d\n",option_e);
#ifdef USE_DBASK
	init_blist();
	init_dbase(option_e+1);
#else
        for(i=1;i<=option_e;i++){
          known[i] = (unsigned char*)malloc_file(abytes(i,cnk[i]),FMODE_RO,DATA_FORMAT,i);
          blist[i] = (uint32_t*)malloc_file(sizeof(uint32_t)*cnk[i],FMODE_RO,BLIST_FORMAT,i);
	}
#endif

	time(&started);

for(level=0;;level++){

tprintf("level=%d\n",level);
	db_t olddb = dbsize;
	mode = EXPAND;

	for(i=lastp;i<olddb;i++) {
		Unpack(dbase[i].db,dbase[i].dw,dbase[i].dd,&w,&b,&d);
		dbase[i].dr  = MoveWhite(w,b,d);
	}

tprintf("dbsize=%u\n",dbsize);

	if(olddb == dbsize)
		break;

	lastp = olddb;
	mode = KLINI;
	uintmax_t total;
	do {
		for(i=0;i<NPROC;i++){
			changed[i]=0;
			pthread_create(pid+i,NULL,thread,(void*)(uintptr_t)i);
		}
		total=0;
		for(i=0;i<NPROC;i++){
			pthread_join(pid[i],NULL);
			total+=changed[i];
		}
		tprintf("\t changed=%ju\n",total);
	}while(total);

	if(dbase[first].dr) break;

}
tprintf("=%d\n",dbase[first].dr);
	mode = SHOW;
	level=0;
	Unpack(dbase[first].db,dbase[first].dw,dbase[first].dd,&w,&b,&d);
	MoveWhite(w,b,d);
	unlink(DBASE_FNAME);
	return 0;
}
