#include "sha.h"
#include <stdio.h>

int main(int argc, char ** argv) {
	TPACK pos;
	if(argc != 2)
		error("./dump <file>");
	FILE * f = fopen(argv[1],"r");
	if(f==NULL)
		error("%s:%m",argv[1]);
	while( fread(&pos,sizeof(pos),1,f) == 1)
		printf("%08X %X %X\n", pos.b, pos.w, pos.d);
	return 0;
}
