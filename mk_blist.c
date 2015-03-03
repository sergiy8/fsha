#include <signal.h>
#include <time.h>
#include "sha.h"
#include "permut.h"
#include "tprintf.h"
#include "malloc_file.c"

#define ALL1(x) ((1<<x) -1 )
#define GINTERVAL 60

static time_t started;
static 	unsigned i,j;
static void glukalo(int s){
    alarm(GINTERVAL);
	tprintf("%d %d\n",i,j);
}


int main(){
	uint32_t * blist = malloc_file(sizeof(uint32_t)<<32l,FMODE_CR,BLIST_NAME);
    time(&started);
    signal(SIGALRM,glukalo);
   alarm(GINTERVAL);
//	__builtin_memset(blist,0xff,sizeof(uint32_t)<<32l);
	for(i=1;i<25;i++) {
		uint32_t x = ALL1(i);
		for(j=0;_popc(x)==i;j++){
			blist[x] = j;
			x=_permut(x);
		}
	}
	blist[0] = BLIST_MAGIC;
	return 0;
}
