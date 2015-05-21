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

static volatile unsigned i[NPROC], j[NPROC];

static void * thread(void * arg){
	unsigned char * job = array;
	const unsigned int n = (uintptr_t)arg;
#if NODAMKA
	i[n]=0; {
#else
	for(i[n]=0;i[n]<(1<<RANK);i[n]++) {
#endif
#if defined(WRANK)
	  if(_popc(i[n])==WRANK)
#endif
		for(j[n]=n;j[n]<(1<<RANK);j[n]+=NPROC){
			kernel((i[n]<<RANK)|j[n], job + JOB_SIZE * j[n]);
		}
		job += JOB_SIZE << RANK;
	}
	tprintf("thread %u closed\n",n);
	return NULL;
}

#define GI (NPROC-1)
static void glukalo(int s){
	alarm(GINTERVAL);
	if (i[GI] >= (1<<RANK))
		tprintf("100%%");
	else
#if WRANK
	tprintf("%s %3X %3X",percent((blist_get(i[GI])<<RANK) | j[GI],cnk(RANK,WRANK)<<RANK),i[GI],j[GI]);
#elif NODAMKA
	tprintf("%s",percent(j[GI],1<<RANK));
#else
	tprintf("%s %2X %2X",percent((i[GI]<<RANK) | j[GI],1<<(2*RANK)),i[GI],j[GI]);
#endif
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

	openlog(argv[0],0,LOG_LOCAL7);
	syslog(LOG_INFO, "started");

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

//#if WRANK || IN_klini || IN_before
//	blist_init();
//#endif

#if   IN_mk_data
#define FMODE FMODE_CR
#elif IN_stat
#define FMODE FMODE_RO
#else
#define FMODE FMODE_RW
#endif

#if NODAMKA
	array = malloc_file(JOB_SIZE<<RANK,FMODE,DATA_FORMAT,RANK);
#else
	array = malloc_file(ARRAY_SIZE_S(RANK),FMODE,DATA_FORMAT,RANK);
#endif

	time(&started);
	signal(SIGALRM,glukalo);

for(;;) {
	alarm(GINTERVAL);
	for(i=0;i<NPROC;i++){
			pthread_create(pid + i,NULL,thread,(void*)(uintptr_t)i);
	}
	for(i=0;i<NPROC;i++){
			pthread_join(pid[i], NULL);
#ifndef IN_stat
			total+=changed[i];
#endif
	}
	alarm(0);
#ifdef IN_klini
	if(total==0) break;
	tprintf("changed=%ju %s\n",total,itoa(total));
	syslog(LOG_INFO,"changed=%ju %s\n",total,itoa(total));
#ifdef WRANK
//	syslog(LOG_INFO,"finished with execl()");
	char k2[PATH_MAX];
	snprintf(k2,sizeof(k2),"./bin/klini%d-%d",RANK,RANK-WRANK);
	execl(k2,k2,NULL);
	panic();
#endif
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
	syslog(LOG_INFO,"finished");
#else
	tprintf("changed=%ju %s\n",total, itoa(total));
	syslog(LOG_INFO,"changed=%ju %s\n", total, itoa(total));
#endif
	PutStat();
	return 0;
}

