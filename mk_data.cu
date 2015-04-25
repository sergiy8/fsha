#include "facecontrol.h"

DATATYPE unsigned char * array;
DATATYPE unsigned long long  changed[CACHESIZE];

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
		if(FaceControl(busy,i,j)) {
		   twobit_set(job,idx,3);
		   atomicAdd(changed + ij%CACHESIZE,1);
			continue;
		}
		if ( i == 0) {
		   twobit_set(job,idx,2);
			continue;
		}
		if ( i == RMASK) {
		   twobit_set(job,idx,1);
			continue;
		}
	}
}

static void PutStat(void){
        FILE * f = fopen(STATFILE(RANK),"w");
        if(f==NULL) return;
        fprintf(f,"mk_data\n");
        fclose(f);
}

