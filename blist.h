
#define BLIST_BITS 24

#if (CNK > (1 << BLIST_BITS))
#error Your need other blist
#endif

struct blist_data {
    int bits:BLIST_BITS;
} __attribute__((packed));

DATATYPE struct blist_data * blist;

#define blist_get(x) blist[x].bits
#define blist_set(x,v) blist[x].bits=v
#define BLIST_SIZE (sizeof(struct blist_data)<<32)

#define BLIST_MAGIC 0x1715
