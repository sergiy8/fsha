#include "sha.h"

#include "../getarg.h"
#include "../qstat.c"
#include "../itoa/itoa.h"

int main(int argc, char ** argv){
	printf("c16_t=%zd potient_t=%zd portion_t=%zd tabsize=%s\n",
		8*sizeof(c16_t),8*sizeof(potient_t),8*sizeof(portion_t),itoa(sizeof(c16)));
	unsigned char job[argc-1];
	int i;
	for(i=1;i<argc;i++){
		job[i-1] = getarg(i);
		printf("%02X\n",job[i-1]);
	}
	uintmax_t count[4] = {0,0,0,0};
	qstat(count,job,argc-1);
	printf("%ju %ju %ju %ju\n",count[0],count[1],count[2],count[3]);
	if( count[0] + count[1] + count[2] + count[3] != (argc-1)*4)
		printf("Laga.\n");
	return 0;
}
