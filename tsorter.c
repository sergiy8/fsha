#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "sha.h"
#include "tcompar.h"
#include "malloc_file.c"

static void safe_write(FILE * f, void * x) {
	if( fwrite(x, sizeof(TPACK), 1 , f) != 1)
		error("fwrite()");
}

int main(int argc, char ** argv) {
	if(argc != 3)
		error("./tsorter <infile> <outfile>");
	struct stat statbuf;
	if (stat(argv[1],&statbuf))
		error("cannot stat %s",argv[1]);
	if( statbuf.st_size % sizeof(TPACK))
		error("size of %s should be multiple %zd",argv[1],sizeof(TPACK));
	char * infile = malloc_file(statbuf.st_size,FMODE_RW,"%s",argv[1]);
	FILE * ofile = fopen(argv[2],"w");
	if (ofile == NULL)
		error("Cannot create %s",argv[1]);

	qsort(infile, statbuf.st_size / sizeof(TPACK), sizeof(TPACK), compar);

	printf("sorted\n");

	safe_write(ofile,infile);

	for( off_t i = sizeof(TPACK); i< statbuf.st_size; i+=sizeof(TPACK))
		if(compar( infile + i - sizeof(TPACK), infile + i))
			safe_write(ofile, infile + i);

	fclose(ofile);

	return 0;
}


