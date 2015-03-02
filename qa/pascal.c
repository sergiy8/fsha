#include <stdio.h>
#include <pascal.inc>

#define RANK 4

int main(){
	int i;
	for(i=0;i<=RANK;i++)
		printf("%d ",PAS(RANK,i));
	printf("\n");

	printf("pas(24,12=%u\n",PAS(24,12));
	
	return 0;
}
