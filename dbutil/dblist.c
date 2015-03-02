#include <sha.h>
#include <fheader.h>
#include <getarg.h>

#include "malloc_file.c"

static int dbfd;
void safe_read(void * addr, size_t sz) {
	if(read(dbfd,addr,sz)!=sz) error("read");
}

static int list = 0;

int main(int argc, char ** argv) {
	unsigned w,d;
	unsigned rank = getarg(1);
	if( argc ==2 ) {
		list =1 ;
	} else {
		w = getarg(2);
		d = getarg(3);
	}
	struct fheader fheader;
	struct dheader dheader;
	char fname[PATH_MAX];
	snprintf(fname,sizeof(fname),DB_FORMAT,rank);
	dbfd = open(fname,O_RDONLY);
	if(dbfd <0) error("Can't open %s",fname);

	safe_read(&dheader,sizeof(dheader));
	if(dheader.magic != DHEADER_MAGIC) error("magic");
	if(dheader.maxo != (1ull<<(2*rank))) error("Illegal rank in %s",fname);


	if (list) {
		error("Not implemented");
	}

	lseek(dbfd,sizeof(dheader)+8*((w<<rank)|d),SEEK_SET);
	uint64_t offset;
	safe_read(&offset,8);
	if(offset==0)
		error("Base does not exists");
	lseek(dbfd,offset,SEEK_SET);

	safe_read(&fheader,sizeof(fheader));
	if(fheader.magic != FHEADER_MAGIC) error("Illegal version");
	if(fheader.rank != rank) error("ill rank");
	if(fheader.w != w) error("ill w");
	if(fheader.d != d) error("ill d");
	if(fheader.wbit) error("It's wbit file, cannot list");
	unsigned idx;
	printf("%c\n","WUU"[fheader.value]);
	for(idx = 0; idx < fheader.count0; idx++ ) {
		uint32_t value;
		safe_read(&value,4);
		printf("%X %X %X\n",value,w,d);
	}
	printf("%c\n","BBW"[fheader.value]);
	for(idx = 0; idx < fheader.count1; idx++ ) {
		uint32_t value;
		safe_read(&value,4);
		printf("%X %X %X\n",value,w,d);
	}
	close(dbfd);
	return 0;
}
