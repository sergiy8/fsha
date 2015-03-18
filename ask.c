#if !defined(IN_before) && ! defined(IN_solver)
#error What?
#endif

#include "malloc_file.c"

DATATYPE unsigned char * known[9];

PROCTYPE int Ask(uint32_t busy, uint32_t iwhite,uint32_t idamka){
	int arank = _popc(busy);
	if(arank==0 || arank>8) error("Error ask");
	uint32_t  idx  = blist_get(busy);
	return twobit_get(known[arank] + (uint64_t)((iwhite<<arank) | idamka) * cnk(32,arank)/4, idx);
}
