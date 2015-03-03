#include "sha.h"
#include "getarg.h"
#include "malloc_file.c"

int main(int argc, char ** argv){

 int rank = getarg(1);

	uint32_t * blist = malloc_file(sizeof(uint32_t)*cnk[rank],FMODE_RO,BLIST_FORMAT,rank);
	uint32_t * nblist = malloc_file(sizeof(uint32_t)<<32l,FMODE_RO,BLIST_NAME);

	if (nblist[0] != BLIST_MAGIC) {
		printf("%s magic error, expected %08X, have %08x\n", BLIST_NAME,BLIST_MAGIC,nblist[0]);
		return -1;
	}
	int i;
	for (i=0;i<cnk[rank];i++)
		if (nblist[blist[i]] != i ) {
			printf("i=%d blist[i]=%08X nblist[blist[i]=%d\n",i,blist[i],nblist[blist[i]]);
		return -1;
	}
	return 0;
}
