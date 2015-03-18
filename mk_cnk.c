#include "sha.h"

// fact(34) can be fit in 128 bits
static __uint128_t  fact( int i) {
    if(i<=1) return 1;
    return fact(i-1) * i;
}

static uint32_t  dcnk(int n, int k) {
    return fact(n)/fact(k)/fact(n-k);
}


int main() {
	int n, k;
	FILE * f = fopen("cnk.inc","w");
	if( f==NULL)
		error("Can't open cnk.inc");
	fprintf(f,
"#ifdef __CUDA_ARCH__\n"
"#error TODO\n"
"#endif\n"
"\n"
"static unsigned long long const cnk_matrix[33][33] = {\n"
	);
	for(n=2;n<33;n++)
	for(k=0;k<=n;k++) {
		fprintf(f,"[%d][%d] = %u,\n",n,k,dcnk(n,k));
	}
	fprintf(f,
"};\n"
"#define cnk(n,k) cnk_matrix[n][k]\n"
"#define CNK cnk(32,RANK)\n"
	);
	fclose(f);
	return 0;
}
