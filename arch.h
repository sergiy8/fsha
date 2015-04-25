#ifdef __CUDA_ARCH__
#define TID() (((uint64_t)blockIdx.y*blockDim.y+blockIdx.x)*blockDim.x + threadIdx.x)
#define DATATYPE static __device__
#define PROCTYPE static __device__
damaged #define KERNEL extern "C" __global__ void kernel(void) {  uint32_t idx = TID(); if(idx>=CNK) return;
#define CACHESIZE (1<<12)
#else
#define DATATYPE static
#define PROCTYPE static
#define KERNEL static void  kernel(unsigned ij, unsigned char * job){
#define atomicAdd(ptr,value) (*(ptr)+=(value))
#define CACHESIZE NPROC
#endif

#ifdef __CUDA_ARCH__
#define _ffs  __ffs
#define _popc __popc
#define _brev __brev
#endif

#if defined(__GNUC__) && ! defined(_ffs)
#define _ffs  __builtin_ffs
#endif
#if defined(__GNUC__) && ! defined(_popc)
#define _popc __builtin_popcount
#endif

#ifndef _popc
static inline uint32_t _popc(uint32_t v){
v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
v = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24; // count
	return v;
}
#endif

#ifndef _brev
static inline uint32_t _brev(uint32_t v){
v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
v = ( v >> 16             ) | ( v               << 16);
	return v;
}
#endif

#define _clz(x) ((x)?__builtin_clz(x):32)
