
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
	uint32_t x,y,z;
	Pack(&x,&y,&z,p.w,p.b,p.d);
	return (TPACK){x,y,z};
}
static inline T12 TUnpack(TPACK p) {
	uint32_t x,y,z;
	Unpack(p.b,p.w,p.d,&x,&y,&z);
	return (T12){x,y,z};
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
