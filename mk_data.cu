#include "facecontrol.h"

DATATYPE unsigned char * array;
DATATYPE uint32_t * busylist;
DATATYPE unsigned long long  changed[CACHESIZE];

KERNEL
	int i;
	unsigned char * job  = array + abytes(RANK,idx);
	uint32_t busy = busylist[idx];

	for(i=0;i<(1<<RANK);i++) {
		twobit_set(job,i,2); // no white - lose
		twobit_set(job,(RMASK<<RANK)+i,1);
	}
	for(i=0;i<(1<<(2*RANK));i++)
		if(FaceControl(busy,i>>RANK,i&RMASK)){
		   twobit_set(job,i,3);
		   atomicAdd(changed + idx%CACHESIZE,1);
		}
}

static void PutStat(void){
        FILE * f = fopen(STATFILE(RANK),"w");
        if(f==NULL) return;
        fprintf(f,"mk_data\n");
        fclose(f);
}

