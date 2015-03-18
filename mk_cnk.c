#include "sha.h"

// fact(34) can be fit in 128 bits
static __uint128_t  fact( int i) __attribute__((const));
static __uint128_t  fact( int i) {
    if(i<=1) return 1;
    return fact(i-1) * i;
}

// 30 bits needed for cnk(32,16)
static uint32_t  dcnk(int n, int k) __attribute__((const));
static uint32_t  dcnk(int n, int k) {
    return fact(n)/fact(k)/fact(n-k);
}


int main(int argc, char ** argv) {
	int n, k;
	FILE * f = fopen("cnk.inc","w");
	if( f==NULL)
		error("Can't open cnk.inc");
	fprintf(f,"//DON'T EDIT - generated by %s "__DATE__" "__TIME__"\n",argv[0]);
	fprintf(f,
"#ifdef __CUDA_ARCH__\n"
"#error TODO\n"
"#endif\n"
"\n"
// TODO: looks like "long long" may be safely removed
"static unsigned long long const cnk_matrix[33][33] = {\n"
	);
	for(n=0;n<33;n++){
		for(k=0;k<=n;k++){
			unsigned val = dcnk(n,k);
			fprintf(f,"[%2d][%2d] = %9u,// %8X %2d\n",n,k,val,val,32-__builtin_clz(val));
		}
		fprintf(f,"\n");
	}
	fprintf(f,
"};\n"
"#define cnk(n,k) cnk_matrix[n][k]\n"
	);
	fclose(f);
	return 0;
}
