#if !defined(IN_before) && ! defined(IN_solver)
#error What?
#endif

#include "malloc_file.c"

DATATYPE unsigned char * known[25];
DATATYPE unsigned char * blist;

PROCTYPE int Ask(uint32_t busy, uint32_t iwhite,uint32_t idamka){
	int arank = _popc(busy);
	if(arank==0) error("Zero ask");
	uint32_t  idx  = blist[busy];
	return twobit_get(known[arank] + ((iwhite<<arank) | idamka) * cnk[arank]/4, idx);
}
