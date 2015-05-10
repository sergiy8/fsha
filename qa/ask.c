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
        array = malloc_file(ARRAY_SIZE_S(rank),FMODE_RO,DATA_FORMAT,rank);
// search index
	for(x=ALLONE(rank);x!=b;x=_permut(x))
		idx++;
	if ( blist_get(b) != idx )
		error("What's fucka with blist\n");
//
	int res = twobit_get(array+(uint64_t)((w<<rank)|d)*cnk(32,rank)/4,idx);
	printf("%08X %X %X = %d\n",b,w,d,res);
	return 0;
}
