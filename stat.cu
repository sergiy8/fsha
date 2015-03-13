static int option_v;
DATATYPE unsigned char * array;

#include "qstat.c"

DATATYPE uintmax_t count[CACHESIZE][4];

KERNEL
	qstat(count[ij%CACHESIZE], array + ARRAY_OFFSET(ij), CNK/4);
}

static void PutStat(void){
	FILE * f;
	f = fopen(STATFILE(RANK),"r");
        if(f) {
              char str[256];
              if(fgets(str,sizeof(str),f))
                  printf("\t%s",str);
              fclose(f);
        } else {
                printf("\n");
        }
}
