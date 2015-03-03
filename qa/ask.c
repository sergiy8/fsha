#include "sha.h"
#include "getarg.h"
#include "permut.h"
#include "malloc_file.c"

int main(int argc, char ** argv){
	uint32_t  x,b,w,d;
	unsigned char * array;
	int rank;
	int idx = 0;
	if(argc!=4) panic();
	b = getarg(1);
	w = getarg(2);
	d = getarg(3);
	rank = _popc(b);
        array = malloc_file(abytes(rank,cnk[rank]),FMODE_RO,DATADIR"%d",rank);
// search index
	for(x=ALLONE(rank);x!=b;x=_permut(x))
		idx++;
	printf("%08X %X %X = %d\n",b,w,d,twobit_get(array+((w<<rank)|d)*cnk[rank]/4,idx));
	return 0;
}
