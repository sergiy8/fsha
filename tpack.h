
#define BITS_FOR_IWHITE 24 // 16  //Analogue MAXRANK

typedef struct {
	uint32_t b;			// Max 24 popped bits
	unsigned w : BITS_FOR_IWHITE;	// Max 12 popped bits
	unsigned d : BITS_FOR_IWHITE;	// Max 24 popped bits
} TPACK;  // 10 bytes

// Slow and accurate - use it for user-inputted data
static inline int TCreate(TPACK * cp, unsigned b, unsigned w, unsigned d) {
	int arank = _popc(b);
	if (arank > BITS_FOR_IWHITE) return -1;
	if ( w > ALLONE(arank)) return -1;
	if ( d > ALLONE(arank)) return -1;
	if (_popc(w) > 12 ) return -1;
	if (_popc(~w & ALLONE(arank)) > 12 ) return -1;
	*cp =  (TPACK){.b=b,.w=w,.d=d};
	return 0;
}

extern int IsForced(TPACK cp);

static inline TPACK TPack(T12 p) {
	uint32_t b2 = p.w | p.b;
	TPACK res = {b2,0,0};
	unsigned i = 1;
	while(b2) {
		int bit = 1<<(_ffs(b2)-1);
		if (p.w&bit)
			res.w |= i;
		if (p.d&bit)
			res.d |= i;
		b2 ^= bit;
		i<<=1;
	}
	return res;

}
static inline T12 TUnpack(TPACK p) {
	T12 res = {0,0,0};
	unsigned i = 1;
	while(p.b) {
		int bit = 1<<(_ffs(p.b)-1);
		if (p.w & i )
			res.w |= bit;
		else
			res.b |= bit;

		if(p.d&i)
			res.d |= bit;

		p.b ^= bit;
		i<<=1;
	}
	return res;
}

#if 0
static inline T12 rev_t12(T12 x ) {
    return (T12){_brev(x.b),_brev(x.w),_brev(x.d)}; // Reverted!
}

#define TRotate(pos) TPack(rev_t12(TUnpack(pos)))
#else
static inline TPACK TRotate(TPACK cp) {
	TPACK res;
	int arank = _popc(cp.b);
	res.b = _brev(cp.b);
	res.w = (_brev(cp.w) >> (32-arank)) ^ ALLONE(arank);
	res.d = _brev(cp.d) >> (32-arank);
	return res;
}
#endif
