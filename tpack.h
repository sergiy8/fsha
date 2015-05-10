
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

#define TFaceControl(cp) FaceControl((cp).b,(cp).w,(cp).d)
static inline void TPack(TPACK * cp, uint32_t w, uint32_t b, uint32_t d) {
	uint32_t x,y,z;
	Pack(&x,&y,&z,w,b,d);
	cp->b = x;
	cp->w = y;
	cp->d = z;
}
#define TUnpack(cp,x,y,z) Unpack(cp.b, cp.w, cp.d, x, y, z)

static inline TPACK TRotate(TPACK cp) {
	TPACK res;
#if 0
	uint32_t ub,uw,ud;
	Unpack(cp.b,cp.w,cp.d,&uw,&ub,&ud);
	uw = _brev(uw);
	ub = _brev(ub);
	ud = _brev(ud);
	TPack(&res,ub,uw,ud);
#else
	int arank = _popc(cp.b);
	res.b = _brev(cp.b);
	res.w = (_brev(cp.w) >> (32-arank)) ^ ALLONE(arank);
	res.d = _brev(cp.d) >> (32-arank);
#endif
	return res;
}
