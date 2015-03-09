// C iz 32 po k
#ifdef __CUDA_ARCH__
__device__
#endif
static unsigned long long const cnk [25] __attribute__ ((unused)) = {
1,
32ull,
32ull*31/2,
32ull*31/2*30/3,
32ull*31/2*30/3*29/4,
32ull*31/2*30/3*29/4*28/5,
32ull*31/2*30/3*29/4*28/5*27/6,
32ull*31/2*30/3*29/4*28/5*27/6*26/7,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13*19/14,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13*19/14*18/15,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13*19/14*18/15*17/16,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13*19/14*18/15,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13*19/14,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12*20/13,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11*21/12,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10*22/11,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9*23/10,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8*24/9,
32ull*31/2*30/3*29/4*28/5*27/6*26/7*25/8,
};

static inline int bitsize(uint32_t x) {
	return 32 - __builtin_clz(x);
}
#define cnksize(x) bitsize(cnk[x]-1)

