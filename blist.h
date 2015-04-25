
// This is an array - convert permutation to number of that permutation

// Some values, number of bits in argument - bits and maxim value in result:
// 8 24	  10518300
// 9 25	  28048800
//10 26	  64512240
//11 27	 129024480
//12 28	 225792840
//13 29	 347373600
//14 29	 471435600
//15 30	 565722720
//16 30	 601080390
//17 30	 565722720

#if RANK > 8
#define BLIST_BITS 32
#else
#define BLIST_BITS 24
#endif

#if BLIST_BITS < 32
#define BLIST_MAGIC 0x1715
struct blist_data {
    unsigned bits:BLIST_BITS;
} __attribute__((packed));
DATATYPE struct blist_data * blist __attribute__((unused));
#define blist_get(x) blist[x].bits
#define blist_set(x,v) blist[x].bits=v

#else
#define BLIST_MAGIC 0x3232
DATATYPE uint32_t * blist __attribute__((unused));
#define blist_get(x) blist[x]
#define blist_set(x,v) blist[x]=v
#endif

#define BLIST_SIZE (sizeof(blist[0])<<32)

#define blist_init() do{blist = malloc_file(BLIST_SIZE,FMODE_RO,BLIST_NAME);}while(0)
