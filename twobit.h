#if RANK > 16
#error Change num to uint64_t
#endif

#define twobit_t unsigned char
#define PER_T (4*sizeof(twobit_t))

static inline int _twobit_get(twobit_t * ptr, uint32_t num){
	unsigned index = (num % PER_T)*2;
	ptr += num/PER_T;
	return (*ptr >> index)&3;
}
static inline void _twobit_set(twobit_t * ptr, uint32_t num, int val){
	unsigned index = (num%PER_T)*2;
	ptr += num/PER_T;
	*ptr |= val << index;
}
static inline void _twobit_clear(twobit_t * ptr, uint32_t num){
	unsigned index = (num%PER_T)*2;
	ptr += num/PER_T;
	*ptr &= ~(3<<index);
}

#define twobit_get(a,b) _twobit_get((twobit_t *)(a),b)
#define twobit_set(a,b,c) _twobit_set((twobit_t *)(a),b,c)
#define twobit_clear(a,b) _twobit_clear((twobit_t *)(a),b)
