// this file intended to be included, not compiled
// all data here is unpacked

#ifndef MOVE_FIND_ALL
#define MOVE_FIND_ALL 0
#endif

#include "neighbor.inc"
static inline int NEX(const int * tab, int *  x){
	if(tab[*x]<0) return 0;
	*x = tab[*x];
	return 1;
}

#define R_NOMOVE (-127)

#define promotion 0xf0000000

#ifndef mprintf
#define mprintf(args...)
#endif

#ifndef IN_klini
// if we can take - do it, otherwise return R_NOMOVE
PROCTYPE int Chain(int i, int dflag, uint32_t mine, uint32_t his, uint32_t d , uint32_t nepusto){
	int x,y;
	int r = R_NOMOVE,r2;
#define ChainDir(TABDIR)\
	for(x=i;NEX(TABDIR,&x);) {\
		if( nepusto & (1<<x)) break;\
		if(!dflag) break;\
	}\
	if( his & (1<<x)) {\
		int rr = R_NOMOVE;\
		for(y=x;(y=TABDIR[y])>=0;) {\
			if( nepusto & (1<<y)) break;\
			r2=Chain(y,dflag||(promotion&(1<<y)),mine,his&~(1<<x),d&~(1<<x), nepusto);\
			if(!MOVE_FIND_ALL && r2>0) return r2;\
			if(r2>rr) rr=r2;\
			if(!dflag) break;\
		}\
		if(rr==R_NOMOVE)\
		for(y=x;NEX(TABDIR,&y);) {\
			if( nepusto & (1<<y)) break;\
			r2 = - MoveBlack(his&~(1<<x),mine|(1<<y),dflag||(promotion&(1<<y))?(d&~(1<<x))|(1<<y):d&~(1<<x));\
			if(!MOVE_FIND_ALL && r2>0) return r2;\
			if(r2>rr) rr = r2;\
			if(!dflag) break;\
		}\
		if(rr>r) r = rr;\
	}
	ChainDir(_UR);
	ChainDir(_UL);
	ChainDir(_DR);
	ChainDir(_DL);
#undef ChainDir
	return r;
}
#endif

PROCTYPE int MoveWhite(uint32_t w, uint32_t b, uint32_t d){
	uint32_t mine;
	int r=R_NOMOVE,r2;
#ifndef IN_klini
	for(mine=w;mine;mine&=mine-1) {
		int i = _ffs(mine) - 1;
		r2 = Chain(i,d&(1<<i),w&~(1<<i),b,d&~(1<<i),w|b);
		if(!MOVE_FIND_ALL && r2>0) return r2;
		if(r2>r) r = r2;
	}
#endif
#if IN_before
	return r ;
#else
	if(r!=R_NOMOVE) return r;
	for(mine=w;mine;mine&=mine-1) {
		int i = _ffs(mine) - 1;
		uint32_t dflag = d & (1<<i);
		int x;
		for(x=i;NEX(_UL,&x);) {
			if( (w|b)&(1<<x)) break;
			r2 = - MoveBlack(b,(w&~(1<<i))|(1<<x),dflag?(d^dflag)|(1<<x):promotion&(1<<x)?d|(1<<x):d);
			if(!MOVE_FIND_ALL && r2>0) return r2;
			if(r2>r) r = r2;
			if(!dflag) break;
		}
		for(x=i;NEX(_UR,&x);) {
			if( (w|b)&(1<<x)) break;
			r2 = - MoveBlack(b,(w&~(1<<i))|(1<<x),dflag?(d^dflag)|(1<<x):promotion&(1<<x)?d|(1<<x):d);
			if(!MOVE_FIND_ALL && r2>0) return r2;
			if(r2>r) r = r2;
			if(!dflag) break;
		}
		if(dflag)
		for(x=i;NEX(_DL,&x);) {
			if( (w|b)&(1<<x)) break;
			r2 = - MoveBlack(b,(w&~(1<<i))|(1<<x),(d^dflag)|(1<<x));
			if(!MOVE_FIND_ALL && r2>0) return r2;
			if(r2>r) r = r2;
			if(!dflag) break;
		}
		if(dflag)
		for(x=i;NEX(_DR,&x);) {
			if( (w|b)&(1<<x)) break;
			r2 = - MoveBlack(b,(w&~(1<<i))|(1<<x),(d^dflag)|(1<<x));
			if(!MOVE_FIND_ALL && r2>0) return r2;
			if(r2>r) r = r2;
			if(!dflag) break;
		}
	}
	return r;
#endif
}
