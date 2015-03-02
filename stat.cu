#include <sys/mman.h> // ;(

#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif


static int option_v;
DATATYPE unsigned char * array;
DATATYPE unsigned * busylist;
DATATYPE int rank;

#include "qstat.c"

DATATYPE uintmax_t count[CACHESIZE][4];

KERNEL
	qstat(count[idx%CACHESIZE], array + abytes(rank,idx), abytes(rank,1));
	if( abytes(rank,1) > PAGE_SIZE)
		munmap(array + abytes(rank,idx),abytes(rank,1));
}

static void PutStat(void){
	FILE * f;
	f = fopen(STATFILE(rank),"r");
        if(f) {
              char str[256];
              if(fgets(str,sizeof(str),f))
                  printf("\t%s",str);
              fclose(f);
        } else {
                printf("\n");
        }
}
