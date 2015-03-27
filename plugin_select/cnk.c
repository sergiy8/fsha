#ifdef INCLUDE_FOR_TABLE
{"cnk",cnk_cmd,.helpline = "C from n by k"},
#else
static void cnk_cmd(const char * arg);
#ifndef INCLUDE_FOR_FORWARD

#include "itoa/itoa.h"
static void cnk_cmd(const char * arg){
	unsigned n,k;
	if( *arg==0 && rank != 0) {
		int i;
		for(i=1;i<rank;i++)
			printf("%llu ", cnk(rank,i));
		printf("\n");
		return;
	}
	if(sscanf(arg,"%u%u",&n,&k)!=2) {
		eprintf("Gimme two args:%s",arg);
		return;
	}
	if( n > 32 || k > n) {
		eprintf("32xk max.");
		return;
	}
	printf("%llu %s\n",cnk(n,k),itoa(cnk(n,k)));
}

#endif
#endif
