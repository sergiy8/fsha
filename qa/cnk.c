#include "sha.h"
#include "itoa/itoa.h"
#include "percent.h"

static long long  fact( int i) {
	if(i==1) return 1;
	return i * fact(i-1);
}

static long long  dcnk(int n, int k) {
	return fact(n)/fact(k)/fact(n-k);
}

int main(){
		int i;
		for(i=1;i<RANK;i++) {
			if(cnk(RANK,i)!=dcnk(RANK,i))
				error("%d %d",i,RANK);
			printf("%d %d %d",RANK,i,(int)cnk(RANK,i));
			if ( i < RANK/2) {
				printf(" %s\n",percent( cnk(RANK,i) + cnk(RANK,RANK-i), 1<<RANK));
			} else if ( i == RANK/2) {
				printf(" %s\n",percent( cnk(RANK,i), 1<<RANK));
			} else {
				printf("\n");
			}
		}
#define DARRAY_SIZE_W(rank,wrank) ((cnk(32,rank)<<(rank))*dcnk(rank,wrank)/4)
		for(i=8;i<14;i++)
				printf("Fsize %d-1=%lld %s\n",i,DARRAY_SIZE_W(i,1),itoa(DARRAY_SIZE_W(i,1)));
        return 0;
}
