#if !defined(IN_before) && ! defined(IN_solver)
#error What?
#endif

#include "search.c"
#include "malloc_file.c"

DATATYPE unsigned char * known[25];
DATATYPE uint32_t * blist[25];

PROCTYPE int Ask(uint32_t busy, uint32_t iwhite,uint32_t idamka){
	int arank = _popc(busy);
#ifdef DEBUG
	if(arank==0) error("Zero ask");
#endif
	uint32_t  idx  = search(busy,blist[arank],cnk[arank]);
	return twobit_get(known[arank] + abytes(arank,idx), (iwhite<<arank)|idamka);
}
