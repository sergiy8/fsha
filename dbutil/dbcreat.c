#include <sha.h>
#include <getarg.h>
#include <fheader.h>
#include <percent.h>

#include "malloc_file.c"


static int dbfd;
void safe_write(void * addr, size_t sz){
	if (write(dbfd,addr,sz)!= sz) error("write");
}


int main(int argc, char ** argv){
	int rank = getarg(1);
	unsigned w,d,idx;
	static struct fheader h = {
		.magic = FHEADER_MAGIC,
	};
	h.rank = rank;

	static struct dheader dheader = {
		.magic = DHEADER_MAGIC,
	};
	dheader.maxo = 1<<(2*rank);

	unsigned char * data = malloc_file(abytes(rank,cnk[rank]),FMODE_RO,DATA_FORMAT,rank);
	uint32_t * blist = malloc_file(sizeof(uint32_t)*cnk[rank],FMODE_RO,BLIST_FORMAT,rank);

#define BITSIZE ((cnk[rank]+31)/32*8)
	uint32_t * bits = malloc(BITSIZE);

	char dbname[PATH_MAX];
	snprintf(dbname,sizeof(dbname),DB_FORMAT,rank);
	unlink(dbname);
	dbfd = open(dbname,O_CREAT|O_RDWR,0666);
	if(dbfd<0) error ("Cannot create %s",dbname);

	safe_write(&dheader,sizeof(dheader));
	uint64_t zero = 0;
	for(idx=0;idx<dheader.maxo;idx++) {
		safe_write(&zero,8);
	}
	uint64_t dbptr = sizeof(dheader) + 8*dheader.maxo;

	for( w = 1; w < (1<<rank)-1; w++)
	for( d = 0; d < (1<<rank); d++) {
		h.w = w;
		h.d = d;
		__builtin_memset(bits,0,BITSIZE);
		uint32_t counts[4]= {0,0,0,0};
		for( idx=0;idx<cnk[rank];idx++){
			unsigned r = twobit_get(data + abytes(rank,idx), (w<<rank) | d);
			twobit_set(bits,idx,r);
			counts[r]++;
		}

		lseek(dbfd,sizeof(dheader)+8*((w<<rank) | d),SEEK_SET);
		safe_write(&dbptr,8);

		uint32_t imax = 0;
		if (counts[1] > counts[0]) imax=1;
		if (counts[2] > counts[imax]) imax=2;
		uint32_t rest = counts[0] + counts[1] + counts[2] - counts[imax];

		if( 4*rest < cnk[rank]/4 ) {
			printf("%d %X %X imax=%d %u %u %u %s\n",
				rank,w,d,imax,counts[0],counts[1],counts[2],
				percent(4*rest,cnk[rank]/4));
			h.wbit = 0;
			h.value = imax;
			h.sorted = 1;
			h.full = 1;
			uint64_t dbptr2;
			switch(imax) {
#define DBW(a,b)\
					h.count0 = counts[a];\
					h.count1 = counts[b];\
					lseek(dbfd,dbptr,SEEK_SET);\
					safe_write(&h,sizeof(h));\
					dbptr += sizeof(h);\
					dbptr2 = dbptr + 4*h.count0;\
					for( idx=0;idx<cnk[rank];idx++)\
						switch(twobit_get(bits,idx)) {\
							default:continue;\
							case a:\
								lseek(dbfd,dbptr,SEEK_SET);\
								safe_write(blist+idx,4);\
								dbptr+=4;\
								continue;\
							case b:\
								lseek(dbfd,dbptr2,SEEK_SET);\
								safe_write(blist+idx,4);\
								dbptr2+=4;\
								continue;\
						}\
					break;
				case 0: DBW(1,2)
				case 1: DBW(0,2)
				case 2: DBW(0,1)
			}
			dbptr = dbptr2;
			continue;
		}
		printf("%d %X %X bits\n",rank,w,d);
		h.wbit = 1;
		lseek(dbfd,dbptr,SEEK_SET);
		safe_write(&h,sizeof(h));
		safe_write(bits,BITSIZE);
		dbptr += sizeof(h) + BITSIZE;
		continue;
	}
	close(dbfd);
	return 0;
}
