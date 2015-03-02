#include "sha.h"
#include "permut.h"

static int rank = MAXRANK-1;
#include "malloc_file.c"
static unsigned char * array;
int main(int argc, char ** argv){
	int idx,i,j;
	uint32_t x;
	if(argc==2) {
		rank = atoi(argv[1]);
	}
	if((rank<=0)||(rank>=MAXRANK)) error("Invalid rank %d , MAXRANK=%d",rank,MAXRANK);
	array = malloc_file(abytes(rank,cnk[rank]),FMODE_RO,DATADIR"%d",rank);

	for(idx=0,x=(1<<rank)-1;idx<cnk[rank];idx++,x=_permut(x))
	for(i=0;i<1<<rank;i++)
	for(j=0;j<1<<rank;j++)
		printf("%08X %X %X =%d\n",x,i,j,
			twobit_get(array+abytes(rank,idx),(i<<rank)|j));
	return 0;
}
