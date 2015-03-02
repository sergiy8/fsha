#include "sha.h"
#include "neighbor.h"

int main(){
	int i;
	printf("static const int _UL[32] = {\n");
	for(i=0;i<32;i++){
		int res;
		if(UL(i,&res))
			printf("%d,",res);
		else
			printf("-1,");
	}
	printf("};\n");
	printf("static const int _UR[32] = {\n");
	for(i=0;i<32;i++){
		int res;
		if(UR(i,&res))
			printf("%d,",res);
		else
			printf("-1,");
	}
	printf("};\n");
	printf("static const int _DL[32] = {\n");
	for(i=0;i<32;i++){
		int res;
		if(DL(i,&res))
			printf("%d,",res);
		else
			printf("-1,");
	}
	printf("};\n");
	printf("static const int _DR[32] = {\n");
	for(i=0;i<32;i++){
		int res;
		if(DR(i,&res))
			printf("%d,",res);
		else
			printf("-1,");
	}
	printf("};\n");
	return 0;
}
