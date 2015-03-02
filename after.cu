#include "facecontrol.h"

DATATYPE unsigned char * array;
DATATYPE unsigned * busylist;
DATATYPE uintmax_t changed[CACHESIZE];

KERNEL
	unsigned ij;
	unsigned char * job  = array + abytes(RANK,idx);
	uint32_t busy = busylist[idx];

	for(ij=0;ij<(1<<2*RANK);ij++){
		if(FaceControl(busy,ij>>RANK,ij&RMASK)==0)
		if(twobit_get(job,ij)==3){
			twobit_clear(job,ij);
			atomicAdd(changed+idx%CACHESIZE,1);
		}
	}
}

#include <unistd.h>
static void PutStat(void){
	unlink(STATFILE(RANK));
}

