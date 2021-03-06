#include "facecontrol.h"

DATATYPE unsigned char * array;
DATATYPE uintmax_t changed[CACHESIZE];
KERNEL
#if NODAMKA
    unsigned i = ij;
    const unsigned j = 0;
#else
    unsigned i = ij >> RANK;
    unsigned j = ij & RMASK;
#endif
    unsigned busy;
    unsigned idx;
    for(idx=0,busy=ALLONE(RANK);_popc(busy)==RANK;idx++,busy = _permut(busy)){
		if(FaceControl((TPACK){busy,i,j})==0)
		if(twobit_get(job,idx)==3){
			twobit_clear(job,idx);
			atomicAdd(changed+ij%CACHESIZE,1);
		}
	}
}
