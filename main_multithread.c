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

static int rank = RANK;

#define LOOP_START (1<<rank)
#define LOOP_END   (ALLONE(rank)<<rank)

static void * threadN(void * arg){
	unsigned ij=(uintptr_t)arg + LOOP_START;
	for(;ij<LOOP_END;ij+=NPROC){
		kernel(ij);
	}
	return NULL;
}

static unsigned ij0;
static void * thread0(void * arg){
	for(ij0=LOOP_START;ij0<LOOP_END;ij0+=NPROC){
		kernel(ij0);
	}
	return NULL;
}

static void glukalo(int s){
	alarm(GINTERVAL);
	tprintf("%s",percent(ij0,1<<(2*rank)));
#ifndef IN_stat
	uintmax_t total = 0;
	int i;
	for(i=0;i<CACHESIZE;i++)
		total+=changed[i];
	printf("\tchg %ju %s\n",total-oldtotal,itoa(total-oldtotal));
	oldtotal=total;
#else
	printf("\n");
#endif
}

static pthread_t pid [NPROC];

static int option_v;
#define usage(prog) error("\n\tUsage: %s [-v] [%d]",prog,rank)
int main(int argc, char ** argv){
	unsigned  i;
        while ((i = getopt(argc, argv, "v")) != -1)
               switch (i) {
		case 'v': option_v++; continue;
		default : usage(argv[0]);
	}
	if(optind!=argc) {
		rank = atoi(argv[optind++]);
#if !IN_stat
		if(rank!=RANK) error("rank %d mandatory",RANK);
#endif
		if(rank>=MAXRANK) error("MAXRANK=%d",MAXRANK);
	}
	if(optind!=argc) usage(argv[0]);

	if(option_v)
		printf("NPROC=%d RANK=%d\n",NPROC,rank);
#if defined(WRANK) && defined(IN_klini)
		printf("WRANK=%d\n",WRANK);
#endif

#if IN_before
        for(i=1;i<rank;i++){
                known[i] = (unsigned char*)malloc_file(abytes(i,cnk[i]),FMODE_RO,DATA_FORMAT,i);
	}
#endif

#if defined(IN_before) || defined(IN_klini)
	blist = malloc_file(BLIST_SIZE,FMODE_RO,BLIST_NAME);
#endif

#ifdef IN_mk_data
	mkdir(DATADIR,0755); errno=0;
	array = malloc_file(abytes(rank,CNK),FMODE_CR,DATADIR"%d",rank);
#else
	array = malloc_file(abytes(rank,cnk[rank]),FMODE_RW,DATADIR"%d",rank);
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
		tprintf("%1d %4ju %4ju %4ju %4ju\n",rank,total[0],total[1],total[2],total[3]);
	printf("%1d %s %s %s %s ",rank,itoa(total[0]),itoa(total[1]),itoa(total[2]),itoa(total[3]));
#if 0
	{ uintmax_t got = total[0] + total[1]+total[2]+total[3];
	  uintmax_t need = ((uintmax_t)cnk[rank]) << (2*rank);
	  if(got!=need) error("got=%ju need=%ju\n",got,need);
	}
#endif
	}
#else
	tprintf("changed=%ju %s\n",total, itoa(total));
#endif
	PutStat();
	return 0;
}

