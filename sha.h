#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAXRANK 9

#if RANK >= MAXRANK
#error MAXRANK too small
#endif

#define ALLONE(x) ((1<<(x)) -1 )
#define RMASK ALLONE(RANK)

#ifndef NPROC
#define NPROC 1
#endif

#include "arch.h"
#include "permut.h"
#include "twobit.h"
#include "cnk.h"
#include "tprintf.h"

#define PATH_MAX 256
#ifndef DATADIR
//#define DATADIR "../data/"
#error Please, define DATADIR in Makefile
#endif
#define DATA_FORMAT DATADIR"%d"
#define BLIST_NAME DATADIR"blist"
#define STATFILE_FORMAT DATADIR"stat%d"
#define STATFILE(r) ({char _loc[256]; snprintf(_loc,sizeof(_loc),STATFILE_FORMAT,r);_loc;})

#define dbg(fmt,args...) fprintf(stderr,"%s:%d:"fmt"\n",__FILE__,__LINE__, ##args)
#define error(fmt,args...) do{fprintf(stderr,errno?"%s:%d:"fmt":%m\n":"%s:%d:"fmt"\n",__FILE__,__LINE__,##args); exit(EXIT_FAILURE);} while(0)

#define panic() error("Panic fucka in %s",__FUNCTION__)


#ifdef __CUDA_ARCH__
#define TID() (((uint64_t)blockIdx.y*blockDim.y+blockIdx.x)*blockDim.x + threadIdx.x)
#define DATATYPE static __device__
#define PROCTYPE static __device__
damaged #define KERNEL extern "C" __global__ void kernel(void) {  uint32_t idx = TID(); if(idx>=CNK) return;
#define CACHESIZE (1<<12)
#else
#define DATATYPE static
#define PROCTYPE static
#define KERNEL static void  kernel(unsigned ij){
#define atomicAdd(ptr,value) (*(ptr)+=(value))
#define CACHESIZE NPROC
#endif
