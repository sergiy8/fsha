static int option_v;
DATATYPE unsigned char * array;

#include "qstat.c"

DATATYPE uintmax_t count[CACHESIZE][4];

KERNEL
	qstat(count[ij%CACHESIZE], job, JOB_SIZE);
//	dbg("i=%d j=%d job=%zd",ij>>RANK, ij&RMASK, job-array);
}
