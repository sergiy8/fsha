#include <stdio.h>
#include "twobit.h"

int main(){
	unsigned x = 0x1715;
printf("x=%x\n",x);
	printf("%d\n",twobit_get(&x,0));
	printf("%d\n",twobit_get(&x,1));
	printf("%d\n",twobit_get(&x,2));
	printf("%d\n",twobit_get(&x,3));
	printf("%d\n",twobit_get(&x,4));
	return 0;
}
