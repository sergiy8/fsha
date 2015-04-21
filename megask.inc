
#include "malloc_file.c"

static unsigned char * known [32];

static void megask_init(void) {
	int i;
	char fname[1024];

	blist = malloc_file(BLIST_SIZE,FMODE_RO,BLIST_NAME);

	for(i=0;i<10;i++) {
		struct stat buf;
		snprintf(fname,sizeof(fname),DATA_FORMAT,i);
		if(stat(fname,&buf))
			continue;
		known[i] = (unsigned char*)malloc_file(ARRAY_SIZE_S(i),FMODE_RO,DATA_FORMAT,i);
	}
}

static int megask(uint32_t busy, uint32_t iwhite,uint32_t idamka) __attribute((const));
static int megask(uint32_t busy, uint32_t iwhite,uint32_t idamka) {
	int arank = __builtin_popcount(busy);
	if(known[arank]==NULL)
		return 4;
    uint32_t  idx  = blist_get(busy);
    return twobit_get(known[arank] + (uint64_t)((iwhite<<arank) | idamka) * cnk(32,arank)/4, idx);
}
