#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include <linux/limits.h> // PATH_MAX

#define ALLONE(x) ((1U<<(x)) -1 )
#define RMASK ALLONE(RANK)

#ifndef NPROC
#define NPROC 1
#endif

enum { R_UNK, R_WIN, R_LOS, R_DRA, R_NOM};

typedef struct {
	uint32_t w,b,d;
} T12; // 12 bytes - unpacked position

#include "arch.h"
#include "permut.h"
#include "twobit.h"
#include "cnk.inc"
#include "tprintf.h"
#include "blist.h"
#include "remote.h"
#include "tpack.h"

#ifndef DATADIR
//#define DATADIR "../data/"
#error Please, define DATADIR in Makefile
#endif


#define DATA_FORMAT DATADIR"%d"
#define DATA_FORMAT_W DATADIR"%d-%d"

#define JOB_SIZE ((uint64_t)(cnk(32,RANK)/4))
#define ARRAY_SIZE_S(rank) (((uint64_t)(cnk(32,rank))<<(2*(rank)))/4)

#define STATFILE_FORMAT DATADIR"stat%d"
#define STATFILE(r) ({char _loc[256]; snprintf(_loc,sizeof(_loc),STATFILE_FORMAT,r);_loc;})

#define dbg(fmt,args...) fprintf(stderr,"%s:%d:"fmt"\n",__FILE__,__LINE__, ##args)
#define error(fmt,args...) do{fprintf(stderr,errno?"%s:%d:"fmt":%m\n":"%s:%d:"fmt"\n",__FILE__,__LINE__,##args); exit(EXIT_FAILURE);} while(0)

#define panic() error("Panic fucka in %s",__FUNCTION__)

