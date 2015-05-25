

int compar(const void * a, const void * b) {
#if 1
	return __builtin_memcmp(a, b, sizeof(TPACK));
#else
	const TPACK * x = (const TPACK*) a;
	const TPACK * y = (const TPACK*) b;

	if(x->b <  y->b) return -1;
	if(x->b >  y->b) return  1;
	if(x->w <  y->w) return -1;
	if(x->w >  y->w) return  1;
	if(x->d <  y->d) return -1;
	if(x->d >  y->d) return  1;
	return 0;
#endif
}
