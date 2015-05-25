#include "sha.h"

#undef MEGASK_REMOTE
#include "megask.c"

static FILE * ofile;

static uintmax_t c_w, c_b, c_l, c_n;

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
		TPACK pos = TPack((T12){w,b,d});
        switch(megask(pos)) {
        case 3:
        case 0 : return 0;
        case 1 : return 5;
        case 2 : return -5;
		case 4:
		if (fwrite(&pos, sizeof(pos), 1, ofile) != 1)
			error("fwrite()");
		c_n++;
    default:
            return 0;
        }
}


PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

#undef NODAMKA
#include "move4.c"

#define INFILE DATADIR"9-q2"
#define OFILE  DATADIR"9-q3.unsorted"

int main() {

	megask_init();

	FILE * infile = fopen(INFILE,"r");
	if(infile == NULL)
		error("Can't open "INFILE);

	ofile = fopen(OFILE,"w");
	if(ofile == NULL)
		error("Can't open "OFILE);

	FILE * wpart = fopen(DATADIR"9-w","a");
	if(wpart == NULL)
		error("Can't open 9-w");
	FILE * bpart = fopen(DATADIR"9-b","a");
	if(bpart == NULL)
		error("Can't open 9-b");

	TPACK pos;
	while(fread(&pos, sizeof(pos), 1, infile)){
		int r = MoveWhite(TUnpack(pos));
		FILE * kuda;
		if (r<0) {
			kuda = bpart;
			c_b++;
		} else if (r>0) {
			kuda = wpart;
			c_w++;
		} else { //takege to draw...
			kuda = ofile;
			c_l++;
		}
		if (fwrite(&pos, sizeof(pos), 1, kuda) != 1)
			error("fwrite()");
	}
	printf("Leave: %ju\n", c_l);
	printf("New: %ju\n", c_n);
	printf("White: %ju\n", c_w);
	printf("Black: %ju\n", c_b);
	return 0;
}
