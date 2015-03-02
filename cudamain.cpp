//#include <string.h>
#include "cudablin/cudablin.h"
#include "malloc_file.c"

CUDAkernel kernel(cubin);

//DATATYPE unsigned char * array;
//DATATYPE uint32_t * busylist;
//DATATYPE uint64_t changed[CACHESIZE];

static unsigned char * array;
static CUdeviceptr host_array,dev_array;

static uint32_t * busylist;
static CUdeviceptr host_busylist,dev_busylist;

static uint64_t changed[CACHESIZE];
static CUdeviceptr dev_changed;

static uintmax_t total;


static void * params[] = {};
static size_t bytes;

#error AskInit

int main(int argc, char ** argv){
	int i;
        if( (argc>=2) && (atoi(argv[1])!=RANK)) error("rank %d mandatory",RANK);
        printf("CUDA RANK=%d\n",RANK);

	kernel.print();
// build busylist
        busylist = (uint32_t*)malloc_file(CNK*sizeof(uint32_t),FMODE_RO,BLIST_FORMAT,RANK);

// put busylist
        SafeCall(cuMemHostRegister(busylist,CNK*sizeof*busylist,CU_MEMHOSTREGISTER_DEVICEMAP));
        SafeCall(cuMemHostGetDevicePointer(&host_busylist,busylist,0));

        SafeCall(cuModuleGetGlobal(&dev_busylist,&bytes,kernel.module[0].module,"busylist"));
        if(bytes!=sizeof(host_busylist)) error("busylist!");
	SafeCall(cuMemcpyHtoD(dev_busylist,&host_busylist,bytes));
// put array
#ifdef IN_mk_data
        mkdir(DATADIR,0755); errno=0;
        array = (unsigned char *)malloc_file(abytes(RANK,CNK),1,DATADIR"%d",RANK);
#else
        array = (unsigned char *)malloc_file(abytes(RANK,CNK),0,DATADIR"%d",RANK);
#endif
        SafeCall(cuMemHostRegister(array,abytes(RANK,CNK),CU_MEMHOSTREGISTER_DEVICEMAP));
        SafeCall(cuMemHostGetDevicePointer(&host_array,array,0));

        SafeCall(cuModuleGetGlobal(&dev_array,&bytes,kernel.module[0].module,"array"));
        if(bytes!=sizeof(host_array)) error("array!");
	SafeCall(cuMemcpyHtoD(dev_array,&host_array,bytes));

#define THREADS 512
#define MAXG    65535
uint64_t nado = (cnk[RANK] +(THREADS-1))/THREADS;
uint32_t gridx = nado>MAXG?MAXG:nado;
uint32_t gridy = (nado+(MAXG-1))/MAXG;
printf("gridy=%d gridx=%d THREAD=%d\n",gridy, gridx, THREADS);

	kernel.launch(params,THREADS,gridx,gridy);
	kernel.wait();

	SafeCall(cuMemHostUnregister(busylist));
	SafeCall(cuMemHostUnregister(array));

        SafeCall(cuModuleGetGlobal(&dev_changed,&bytes,kernel.module[0].module,"changed"));
        if(bytes!=sizeof(changed)) error("changed!");
	SafeCall(cuMemcpyDtoH(changed,dev_changed,bytes));

	for(i=0;i<CACHESIZE;i++)
		total += changed[i];
	printf("changed=%ju\n",total);

	return 0;
}
