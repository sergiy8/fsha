#include <time.h>
#include "sha.h"
#include "pack.h"

static time_t started;

typedef uint32_t db_t;
struct ITEM {
db_t left, right;
uint32_t db;
unsigned dw:24,dd:24;
} __attribute__((packed));
static struct ITEM * dbase;
static int db_compar(db_t  a, db_t b){
        struct ITEM *xa = dbase + a;
        struct ITEM *xb = dbase + b;
        if (xa->db < xb->db) return -1;
        if (xa->db > xb->db) return  1;
//        if (xa->dw < xb->dw) return -1;
//        if (xa->dw > xb->dw) return  1;
        if (xa->dd < xb->dd) return -1;
        if (xa->dd > xb->dd) return  1;
        return 0;
}
#define DBSIZE ((db_t)(1<<31))
#include "db.c"

static db_t rootp[1<<24];


static int level;
static db_t lastp;

int MoveBlack(uint32_t w, uint32_t b, uint32_t d){
	if(_popc(w|b) != 24 ) return -1;
        uint32_t busy,iwhite,idamka;
        Pack(&busy,&iwhite,&idamka,_brev(w),_brev(b),_brev(d));
        db_t x = db_alloc();
        dbase[x].db = busy;
        dbase[x].dw = iwhite;
        dbase[x].dd = idamka;
        if(tsearch(x,rootp+iwhite)!=x)
                db_free();
	    return 0;
}
#include "move4.c"

int main(int argc, char ** argv){
	db_t i;
	db_t first;

	if(argc>1) {
		i = atoi(argv[1]);
		dbase = malloc_file(sizeof(struct ITEM)*DBSIZE,FMODE_RO,"dbase24");
		printf("%08X %X %X\n",dbase[i].db,dbase[i].dw,dbase[i].dd);
		return 0;
	}

dbase = malloc_file(sizeof(struct ITEM)*DBSIZE,FMODE_CR,"dbase24");

	lastp = dbsize;
	first = db_alloc();
	dbase[first].db = 0xfff00fff;
	dbase[first].dw = 0x00000fff;
	dbase[first].dd = 0;

	rootp[dbase[first].dw] = first;

	time(&started);

for(level=0;;level++){
	tprintf("level=%d tail=%d dbsize=%u\n",level,dbsize-lastp,dbsize);
	    db_t newp =  dbsize;
		for(;lastp!=newp;lastp++){
			uint32_t w,b,d;
                Unpack(dbase[lastp].db,dbase[lastp].dw,dbase[lastp].dd,&w,&b,&d);
                MoveWhite(w,b,d);
        }
        if(dbsize==newp) break;
}
	return 0;
}
