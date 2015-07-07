#include "time.h"
#include "signal.h"
#include "sha.h"
#include "percent.h"

#undef MEGASK_REMOTE
#include "megask.c"

static time_t started;
static volatile uintmax_t curindex, maxindex;
#define GINTERVAL 60
static void glukalo(int s){
	alarm(GINTERVAL);
	tprintf("%s\n",percent(curindex, maxindex));
}

static FILE * ofile;

static uintmax_t c_w, c_b, c_l, c_n;

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
		TPACK pos = TPack((T12){w,b,d});
        switch(megask(pos)) {
		case ASK_UNK: return R_UNK;
        case ASK_DRAW : return R_DRA;
        case ASK_WHITE : return R_WIN;
        case ASK_BLACK : return R_LOS;
		case ASK_NODB:
			if(pos.d == 0)
				error("Smth wrong");
			if (fwrite(&pos, sizeof(pos), 1, ofile) != 1)
				error("fwrite()");
			c_n++;
			return R_UNK;
    	default:
			error("Smth wrong from megask");
        }
}


PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

#undef NODAMKA
#include "move5.c"

static char * INFILE  = DATADIR"9-q";
#define OFILE  DATADIR"9-q-qprocessor"

int main(int argc, char **argv) {

	if( argc == 2)
		INFILE = argv[1];
	dbg("INFILE=%s",INFILE);

	megask_init();

	struct stat buf;
	if( stat(INFILE,&buf ))
		error("Cannot stat %s:%m",INFILE);

	if (buf.st_size % sizeof(TPACK))
		error("Illegal size %s",INFILE);
	maxindex = buf.st_size / sizeof(TPACK);

	FILE * infile = fopen(INFILE,"r");
	if(infile == NULL)
		error("Can't open %s",INFILE);

	ofile = fopen(OFILE,"w");
	if(ofile == NULL)
		error("Can't open "OFILE);

	FILE * wpart = fopen(DATADIR"9-w-qprocessor","w");
	if(wpart == NULL)
		error("Can't open 9-w");
	FILE * bpart = fopen(DATADIR"9-b-qprocessor","w");
	if(bpart == NULL)
		error("Can't open 9-b");

	TPACK pos;

    time(&started);
    signal(SIGALRM,glukalo);
	alarm(GINTERVAL);

	while(fread(&pos, sizeof(pos), 1, infile)){
		curindex ++;
		int r = MoveWhite(TUnpack(pos));
		FILE * kuda;
		if (r==R_LOS) {
			kuda = bpart;
			c_b++;
		} else if (r==R_WIN) {
			kuda = wpart;
			c_w++;
		} else { //We don't copy unresolved to output
			kuda = NULL;
			c_l++;
		}
		if(kuda)
			if (fwrite(&pos, sizeof(pos), 1, kuda) != 1)
				error("fwrite()");
	}
	printf("Leave: %ju\n", c_l);
	printf("New: %ju\n", c_n);
	printf("White: %ju\n", c_w);
	printf("Black: %ju\n", c_b);
	return 0;
}
