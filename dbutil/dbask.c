#ifdef WITH_MAIN
#include "../sha.h"
#endif
#include "../fheader.h"
#include "../getarg.h"
#include "../malloc_file.c"
#include "../search.c"
static uint32_t * blist[MAXRANK];
static void init_blist(void){
	int rank;
	for(rank=1;rank<MAXRANK;rank++)
		blist[rank] = malloc_file(sizeof(uint32_t)*cnk[rank],FMODE_RO,BLIST_FORMAT,rank);
}

static struct dheader *  db[MAXRANK];
#define dbknown(rank) (db[rank]!=NULL)
static void init_dbase(int maxrank){
	int rank;
	char fname[PATH_MAX];
	for(rank=1;rank<maxrank;rank++) {
		struct stat buf;
		snprintf(fname,sizeof(fname),DB_FORMAT,rank);
		if(stat(fname,&buf)) continue;
		db[rank] = malloc_file(buf.st_size,FMODE_RO,"%s",fname);
		//printf("%s in use\n",fname);
	}
}

static const unsigned r0[3] = {1,0,0};
static const unsigned r1[3] = {2,2,1};
static unsigned dbget_raw(struct fheader * h, unsigned busy){
	uint32_t idx;
	if( h->wbit){
	 idx = search(busy,blist[h->rank],cnk[h->rank]);
	 return  twobit_get(h->data,idx);
	}
	if (h->sorted){
		idx = search(busy,(uint32_t *)(h->data),h->count0);
		if (idx != -1)
			return r0[h->value];
		idx = search(busy,(uint32_t *)(h->data)+h->count0,h->count1);
		if (idx != -1)
			return r1[h->value];
	}else {
		for(idx=0;idx<h->count0;idx++)
			if(((uint32_t*)(h->data))[idx] == busy)
				return r0[h->value];
		for(;idx<h->count0+h->count1;idx++)
			if(((uint32_t*)(h->data))[idx] == busy)
				return r1[h->value];
	}
	if (h->full){
		return h->value;
	}
	error("Not in db");
}

unsigned dbask(unsigned busy,unsigned w , unsigned d) {
	int rank = _popc(busy);
	if (w==0) return 0;
	if (_popc(w) == rank) return 1;
	if (db[rank]==NULL) error(DB_FORMAT,rank);
	uint64_t offset = (db[rank]->offsets)[(w<<rank)|d];
	if(offset==0)
		error("No file in "DB_FORMAT", should not happen",rank);
	return dbget_raw( (struct fheader*)(((unsigned char*)(db[rank])) + offset), busy);
}

#ifdef WITH_MAIN

#define RANK _popc(busy)
#include "../facecontrol.h"

int main(int argc, char ** argv) {
	int busy = getarg(1);
	int w = getarg(2);
	int d = getarg(3);
	printf("%X %X %X\n",busy,w,d);
	if(FaceControl(busy,w,d))
		error("Stopped by face control");
	init_blist();
	init_dbase(MAXRANK);
	printf("%d\n",dbask(busy,w,d));
	return 0;
}
#endif
