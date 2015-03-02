#include "sha.h"

int pascal[33][33];

int main(){
	int i,j;
	FILE * f;
	f = fopen("pascal.inc","w");
	if(f==NULL) panic();
	pascal[0][0]=1;
	for(i=1;i<33;i++) {
		pascal[i][0]=1;
		for(j=1;j<i;j++)
		   pascal[i][j] = pascal[i-1][j-1] + pascal[i-1][j];
		pascal[i][j]=1;
	}
	
	for(i=1;i<33;i++){
		fprintf(f,"#define _PAS%d(_x) \\\n",i);
		for(j=0;j<i;j++)
		   fprintf(f,"(_x)==%d?%d:\\\n",j,pascal[i][j]);
		fprintf(f,"1\n");
	}
	fprintf(f,"#define _PAS(_i,_j) _PAS##_i(_j)\n");
	fprintf(f,"#define PAS(_ii,_jj) _PAS(_ii,_jj)\n");
	fprintf(f,"#define cnk(_k) PAS(32,_k)\n");
	fclose(f);
	return 0;
}
