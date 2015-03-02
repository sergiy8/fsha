
PROCTYPE inline void Pack( uint32_t * busy, uint32_t * iwhite, uint32_t * idamka,  uint32_t w, uint32_t b, uint32_t d) {
	uint32_t b2 = w | b;
	unsigned i=1;
	*busy = b2;
	*iwhite = 0;
	*idamka = 0;
	while(b2) {
	   if( w & (1<<(_ffs(b2)-1)))
		*iwhite |= i;
	   if( d & (1<<(_ffs(b2)-1)))
		*idamka |= i;
	   b2 &= b2-1;
	   i<<=1;
	}
}
PROCTYPE inline void Unpack( uint32_t busy, uint32_t iwhite, uint32_t idamka,  uint32_t * w, uint32_t * b, uint32_t * d) {
	*w = 0;
	*b = 0;
	*d = 0;
	unsigned i = 1;
	while(busy) {
	    uint32_t bit =1 << (_ffs(busy)-1);
	    * ( (iwhite&i)?w:b) |= bit;
	    if(idamka & i )
		*d |= bit;
	    busy &= ~bit;
	    i<<=1;
	}
}
