#include "sha.h"
#include "permut.h"
#include "malloc_file.c"

int main(){
	unsigned i,j;
	for(i=1;i<MAXRANK;i++) {
		uint32_t * blist = malloc_file(sizeof(uint32_t)*cnk[i],FMODE_CR,BLIST_FORMAT,i);
		uint32_t x = (1<<i)-1;
		for(j=0;j<cnk[i];j++) {
			blist[j]=x;
			x = _permut(x);
		}
		munmap(blist,sizeof(uint32_t)*cnk[i]);
		printf(BLIST_FORMAT" done\n",i);
	}
	return 0;
}
