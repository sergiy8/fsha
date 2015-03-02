
#include "c16.inc"

#define ARG_MASK ((1<<ARG_SHIFT)-1)

// we have always 4 virtual machines
// size of each in bits:
#define MSIZE (2*sizeof(c16_t))
#define ONE ((c16_t)1)
#define MMASK ((ONE<<MSIZE)-1)
#define OVF ((ONE<<(MSIZE-1)) | (ONE<<(2*MSIZE-1)) | (ONE<<(3*MSIZE-1)) | (ONE<<(4*MSIZE-1)))

//typedef __uint128_t portion_t;
typedef uint64_t portion_t;
//typedef potient_t portion_t;

static c16_t Potient(potient_t x){
    return c16[x];
}
static c16_t Portion(portion_t x){
    c16_t res = 0;
    int p;
    for(p=0;p<sizeof(portion_t)/sizeof(potient_t);p++)
        res += Potient(x >> (p*8*sizeof(potient_t)));
    return res;
}



void qstat(uintmax_t count[4],uint8_t * job, size_t sz){
	c16_t accum = 0;
	while(sz>=sizeof(portion_t)) {
		accum += Portion(*(portion_t *) job);
		job += sizeof(portion_t);
		sz -= sizeof(portion_t);
		if ((accum & OVF) == (c16_t)0)
			continue;
		count[0] += accum & MMASK;
		count[1] += (accum >> MSIZE ) & MMASK;
		count[2] += (accum >> (2*MSIZE )) & MMASK;
		count[3] += accum >> (3*MSIZE );
		accum = 0;
	}
	while(sz--)
		accum += Portion(*job++) - 4*(sizeof(portion_t)-1);
	count[0] += accum & MMASK;
	count[1] += (accum >> MSIZE ) & MMASK;
	count[2] += (accum >> (2*MSIZE )) & MMASK;
	count[3] += accum >> (3*MSIZE );
}
