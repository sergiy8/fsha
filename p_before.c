#include "sha.h"

#undef MEGASK_REMOTE
#include "megask.c"

PROCTYPE int StaticWhite(uint32_t w, uint32_t b, uint32_t d){
        switch(megask(TPack((T12){w,b,d}))) {  // RANK of ask is always less the our
        case 3:
        case 0 : return 0;
        case 1 : return 5;
        case 2 : return -5;
    default:
            return 0;
        }
}


PROCTYPE inline int MoveBlack(T12 pos){
        return StaticWhite(_brev(pos.w),_brev(pos.b),_brev(pos.d));
}

#define IN_before 1
#include "move4.c"

int main() {

	megask_init();

	FILE * infile = fopen(DATADIR"9-q1","r");
	if(infile == NULL)
		error("Can't open 9-q1");
	FILE * ofile = fopen(DATADIR"9-q2","w");
	if(ofile == NULL)
		error("Can't open 9-q2");
	FILE * wpart = fopen(DATADIR"9-w","w");
	if(wpart == NULL)
		error("Can't open 9-w");
	FILE * bpart = fopen(DATADIR"9-b","w");
	if(bpart == NULL)
		error("Can't open 9-b");
	FILE * dpart = fopen(DATADIR"9-d","w");
	if(dpart == NULL)
		error("Can't open 9-d");

	TPACK pos;
	while(fread(&pos, sizeof(pos), 1, infile)){
		int r = MoveWhite(TUnpack(pos));
		FILE * kuda;
		if(r==R_NOMOVE) {
			kuda = ofile;
		} else if (r<0) {
			kuda = bpart;
		} else if (r>0) {
			kuda = wpart;
		} else { //takege to draw...
			kuda = dpart;
		}
		if (fwrite(&pos, sizeof(pos), 1, kuda) != 1)
			error("fwrite()");
	}
	return 0;
}
