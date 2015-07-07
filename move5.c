// this file intended to be included, not compiled
// all data here is unpacked

#ifndef MOVE_FIND_ALL
#define MOVE_FIND_ALL 0
#endif

#ifndef _MOVE5_C
#define _MOVE5_C

#include "neighbor.inc"
static inline int NEX(const int * tab, int *  x){
	if(tab[*x]<0) return 0;
	*x = tab[*x];
	return 1;
}

#define promotion 0xf0000000

static const int  restable[5][5] = {
	[R_UNK][R_UNK] = R_UNK,
	[R_UNK][R_WIN] = R_WIN,
	[R_UNK][R_LOS] = R_UNK,
	[R_UNK][R_DRA] = R_DRA,
	[R_UNK][R_NOM] = R_UNK,

	[R_WIN][R_UNK] = R_WIN,
	[R_WIN][R_WIN] = R_WIN,
	[R_WIN][R_LOS] = R_WIN,
	[R_WIN][R_DRA] = R_WIN,
	[R_WIN][R_NOM] = R_WIN,

	[R_LOS][R_UNK] = R_UNK,
	[R_LOS][R_WIN] = R_WIN,
	[R_LOS][R_LOS] = R_LOS,
	[R_LOS][R_DRA] = R_DRA,
	[R_LOS][R_NOM] = R_LOS,

	[R_DRA][R_UNK] = R_UNK,
	[R_DRA][R_WIN] = R_WIN,
	[R_DRA][R_LOS] = R_DRA,
	[R_DRA][R_DRA] = R_DRA,
	[R_DRA][R_NOM] = R_DRA,

	[R_NOM][R_UNK] = R_UNK,
	[R_NOM][R_WIN] = R_WIN,
	[R_NOM][R_LOS] = R_LOS,
	[R_NOM][R_DRA] = R_DRA,
	[R_NOM][R_NOM] = R_NOM,
};

#define r_update(old,new) old = restable[old][new]
static const int  resnegate[5]= {
	[R_UNK] = R_UNK,
	[R_WIN] = R_LOS,
	[R_LOS] = R_WIN,
	[R_DRA] = R_DRA,
	[R_NOM] = R_WIN,
};
#define r_negate(x) resnegate[x]


#endif //_MOVE5_C

#ifndef IN_klini
// if we can take - do it, otherwise return R_NOM
PROCTYPE int Chain(int i, int dflag, uint32_t mine, uint32_t his, uint32_t d , uint32_t nepusto){
	int x,y;
	int r = R_NOM,r2;
#define ChainDir(TABDIR)\
	for(x=i;NEX(TABDIR,&x);) {\
		if( nepusto & (1<<x)) break;\
		if(!dflag) break;\
	}\
	if( his & (1<<x)) {\
		int rr = R_NOM;\
		for(y=x;(y=TABDIR[y])>=0;) {\
			if( nepusto & (1<<y)) break;\
			r2=Chain(y,dflag||(promotion&(1<<y)),mine,his&~(1<<x),d&~(1<<x), nepusto);\
			if(!MOVE_FIND_ALL && r2==R_WIN) return r2;\
			r_update(rr,r2);\
			if(!dflag) break;\
		}\
		if(rr==R_NOM)\
		for(y=x;NEX(TABDIR,&y);) {\
			if( nepusto & (1<<y)) break;\
			r2 = r_negate(MoveBlack((T12){his&~(1<<x),mine|(1<<y),dflag||(promotion&(1<<y))?(d&~(1<<x))|(1<<y):d&~(1<<x)}));\
			if(!MOVE_FIND_ALL && r2==R_WIN) return r2;\
			r_update(rr,r2);\
			if(!dflag) break;\
		}\
		r_update(r,rr);\
	}
	ChainDir(_UR);
	ChainDir(_UL);
	ChainDir(_DR);
	ChainDir(_DL);
#undef ChainDir
	return r;
}
#endif

PROCTYPE int MoveWhite(T12 pos){
	uint32_t mine;
	int r=R_NOM,r2;
#ifndef IN_klini
	for(mine=pos.w;mine;mine&=mine-1) {
		int i = _ffs(mine) - 1;
		r2 = Chain(i,pos.d&(1<<i),pos.w&~(1<<i),pos.b,pos.d&~(1<<i),pos.w|pos.b);
		if(!MOVE_FIND_ALL && r2==R_WIN) return r2;
		r_update(r,r2);
	}
#endif
#if IN_before
	return r ;
#else
	if(r!=R_NOM) return r;
	for(mine=pos.w;mine;mine&=mine-1) {
		int i = _ffs(mine) - 1;
#if IN_klini && NODAMKA
		const uint32_t dflag = 0;
#else
		uint32_t dflag = pos.d & (1<<i);
#endif
		int x;
		for(x=i;NEX(_UL,&x);) {
			if( (pos.w|pos.b)&(1<<x)) break;
			r2 = r_negate(MoveBlack((T12){pos.b,(pos.w&~(1<<i))|(1<<x),dflag?(pos.d^dflag)|(1<<x):pos.d|(promotion&(1<<x))}));
			if(!MOVE_FIND_ALL && r2==R_WIN) return R_WIN;
			r_update(r,r2);
			if(!dflag) break;
		}
		for(x=i;NEX(_UR,&x);) {
			if( (pos.w|pos.b)&(1<<x)) break;
			r2 = r_negate(MoveBlack((T12){pos.b,(pos.w&~(1<<i))|(1<<x),dflag?(pos.d^dflag)|(1<<x):pos.d|(promotion&(1<<x))}));
			if(!MOVE_FIND_ALL && r2==R_WIN) return R_WIN;
			r_update(r,r2);
			if(!dflag) break;
		}
		if(dflag)
		for(x=i;NEX(_DL,&x);) {
			if( (pos.w|pos.b)&(1<<x)) break;
			r2 = r_negate(MoveBlack((T12){pos.b,(pos.w&~(1<<i))|(1<<x),(pos.d^dflag)|(1<<x)}));
			if(!MOVE_FIND_ALL && r2==R_WIN) return R_WIN;
			r_update(r,r2);
		}
		if(dflag)
		for(x=i;NEX(_DR,&x);) {
			if( (pos.w|pos.b)&(1<<x)) break;
			r2 = r_negate(MoveBlack((T12){pos.b,(pos.w&~(1<<i))|(1<<x),(pos.d^dflag)|(1<<x)}));
			if(!MOVE_FIND_ALL && r2==R_WIN) return R_WIN;
			r_update(r,r2);
		}
	}
	return r;
#endif
}
