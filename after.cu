#include "facecontrol.h"

DATATYPE unsigned char * array;
DATATYPE uintmax_t changed[CACHESIZE];
KERNEL
    unsigned i = ij >> RANK;
    unsigned j = ij & RMASK;
    unsigned busy;
    unsigned idx;
    unsigned char * job = array + ij * CNK /4;
    for(idx=0,busy=ALLONE(RANK);_popc(busy)==RANK;idx++,busy = _permut(busy)){
		if(FaceControl(busy,i,j)==0)
		if(twobit_get(job,idx)==3){
			twobit_clear(job,idx);
			atomicAdd(changed+ij%CACHESIZE,1);
		}
	}
}

#include <unistd.h>
static void PutStat(void){
	unlink(STATFILE(RANK));
}

