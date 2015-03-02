#include <unistd.h>
#include "sha.h"

#include "permut.h"

#include "getarg.h"

int main(int argc, char ** argv){
	uint32_t x;
	int rank=0,opt;
        while ((opt = getopt(argc, argv, "r:")) != -1)
	switch(opt) {
		default: error("Invalid option");
		case 'r': rank = atoi(optarg);
			  continue;
	}
//printf("optind=%d\n",optind);
	argv+=optind-1;
	argc-=optind-1;
	if(rank) {
		for(x=(1<<rank)-1;x!=0xffffffff;x=_permut(x))
	         	printf("%08X\n",x);
		return 0;
	}
	x =  getarg(1);
	printf("%08X %08X\n",x,_permut(x));
	return 0;
}
