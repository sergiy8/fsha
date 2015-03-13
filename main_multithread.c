#include <pthread.h>
#include <time.h>
#include <unistd.h> // getopt
#include <signal.h> // getopt
#include "itoa/itoa.h"
#include "malloc_file.c"
#include "percent.h"

#define GINTERVAL 60

static time_t started;

#ifndef IN_stat
static uintmax_t total;
static uintmax_t oldtotal; // for glukalo
#endif

static void * threadN(void * arg){
	unsigned i,j;
	for(j=(uintptr_t)arg;j<(1<<RANK);j+=NPROC)
#if RANK > 8
	for(i=ALLONE(WRANK);i<(1<<WRANK);i=_permut(i))
#else
	for(i=0;i<(1<<RANK);i++)
#endif
		kernel((i<<RANK)|j);
	return NULL;
}

static unsigned j0;
static void * thread0(void * arg){
	unsigned i,j;
	for(j=0;j<(1<<RANK);j+=NPROC)
#if RANK > 8
	for(i=ALLONE(WRANK);i<(1<<WRANK);i=_permut(i))
#else
	for(i=0;i<(1<<RANK);i++)
#endif
		kernel((i<<RANK)|j);
	return NULL;
}

static void glukalo(int s){
	alarm(GINTERVAL);
	tprintf("%s",percent(j0,1<<WRANK));
#ifndef IN_stat
	uintmax_t total = 0;
	int i;
	for(i=0;i<CACHESIZE;i++)
		total+=changed[i];
	fprintf(stderr,"\tchg %ju %s\n",total-oldtotal,itoa(total-oldtotal));
	oldtotal=total;
#else
	fprintf(stderr,"\n");
#endif
}

static pthread_t pid [NPROC];

static int option_v;
#define usage(prog) error("\n\tUsage: %s [-v]",prog)
int main(int argc, char ** argv){
	unsigned  i;
        while ((i = getopt(argc, argv, "v")) != -1)
               switch (i) {
		case 'v': option_v++; continue;
		default : usage(argv[0]);
	}
	if(optind!=argc) usage(argv[0]);

	if(option_v)
		printf("NPROC=%d RANK=%d\n",NPROC,RANK);
#if defined(WRANK) && defined(IN_klini)
		printf("WRANK=%d\n",WRANK);
#endif

#if IN_before
        for(i=1;i<RANK;i++){
                known[i] = (unsigned char*)malloc_file(ARRAY_SIZE_S(i),FMODE_RO,DATA_FORMAT,i);
	}
#endif

	blist = malloc_file(BLIST_SIZE,FMODE_RO,BLIST_NAME);

#if   IN_mkdata
#define FMODE FMODE_CR
#elif IN_stat
#define FMODE FMODE_RO
#else
#define FMODE FMODE_RW
#endif

#if RANK > 8
	array = malloc_file(ARRAY_SIZE_W(RANK,WRANK),FMODE,DATA_FORMAT_W,RANK,WRANK);
#else
	array = malloc_file(ARRAY_SIZE_S(RANK),FMODE,DATA_FORMAT,RANK);
#endif

	time(&started);
	signal(SIGALRM,glukalo);

for(;;) {
	alarm(GINTERVAL);
	for(i=0;i<NPROC;i++){
			pthread_create(pid + i,NULL,i?threadN:thread0,(void*)(uintptr_t)i);
	}
	for(i=0;i<NPROC;i++){
			pthread_join(pid[i], NULL);
#ifndef IN_stat
			total+=changed[i];
#endif
	}
#ifdef IN_klini
	if(total==0) break;
	tprintf("changed=%ju %s\n",total,itoa(total));
	oldtotal=0;
	total=0;
	for(i=0;i<NPROC;i++)
		changed[i]=0;
#else
	break;
#endif
}
#ifdef IN_stat
	{ uintmax_t total[4] = {0};
	  for(i=0;i<CACHESIZE;i++){
		total[0] += count[i][0];
		total[1] += count[i][1];
		total[2] += count[i][2];
		total[3] += count[i][3];
	  }
	if(option_v)
		tprintf("%1d %4ju %4ju %4ju %4ju\n",RANK,total[0],total[1],total[2],total[3]);
	printf("%1d %s %s %s %s ",RANK,itoa(total[0]),itoa(total[1]),itoa(total[2]),itoa(total[3]));
	}
#else
	tprintf("changed=%ju %s\n",total, itoa(total));
#endif
	PutStat();
	return 0;
}

