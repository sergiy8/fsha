#include "sha.h"

static long long  fact( int i) {
	if(i==1) return 1;
	return i * fact(i-1);
}

static long long  dcnk(int n, int k) {
	return fact(n)/fact(k)/fact(n-k);
}

int main(){
		printf("cnk(8,3)=%lld %lld\n",cnk(8,3), dcnk(8,3));
        return 0;
}
