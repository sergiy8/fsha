#include <inttypes.h>
#include "sha.h"

// The array size will be sizeof(c16_t)<<ARG_SHIFT
#define ARG_SHIFT 16

#if 0 // change it to 1 for middle case
typedef uint64_t c16_t;
#define RESNAME "uint64_t"
#define data2file(f,x) fprintf(f,"0x%"PRIX64"llu,",x);
#elif __SIZEOF_INT128__
typedef __uint128_t c16_t;
#define RESNAME "__uint128_t"
#define data2file(f,x) fprintf(f,"(((c16_t)0x%"PRIX64"llu)<<64)|0x%"PRIX64"llu,",(uint64_t)(x>>64),(uint64_t)x);
#else
typedef uint32_t c16_t;
#define RESNAME "uint32_t"
#define data2file(f,x) fprintf(f,"0x%"PRIX32",",x);
#endif

#define M0 1
#define M1 ((c16_t)1<<(2*sizeof(c16_t)))
#define M2 ((c16_t)1<<(4*sizeof(c16_t)))
#define M3 ((c16_t)1<<(6*sizeof(c16_t)))

static c16_t c16[1<<ARG_SHIFT];

int main(){
	FILE * f;
	int i,j,p;
	for(i=0;i<(1<<ARG_SHIFT);i++)
	for(j=0;j<ARG_SHIFT;j+=2) {
		p =  ( i >> j ) & 3;
		switch(p) {
		case 0 : c16[i]+=M0; break;
		case 1 : c16[i]+=M1; break;
		case 2 : c16[i]+=M2; break;
		case 3 : c16[i]+=M3; break;
		}
	}
printf("ARG_SHIFT=%d c16_t=%s\n",ARG_SHIFT,RESNAME);
	f = fopen("c16.inc","w");
	if(f==NULL) panic();
	fprintf(f,"#define ARG_SHIFT %d\n",ARG_SHIFT);
	fprintf(f,"typedef %s c16_t;\n",RESNAME);
	fprintf(f,"typedef uint%d_t potient_t;\n",ARG_SHIFT);
	fprintf(f,"static const c16_t c16[1<<ARG_SHIFT] = {\n");
	for(i=0;i<(1<<ARG_SHIFT);i++) {
		data2file(f,c16[i]);
		if((i%16==15)) fprintf(f,"\n");
	}
	fprintf(f,"};\n");
	fclose(f);
	return 0;
}
