#if !defined(IN_before) && ! defined(IN_solver)
#error What?
#endif

#include "malloc_file.c"
#include "blist.h"

DATATYPE unsigned char * known[25];

PROCTYPE int Ask(uint32_t busy, uint32_t iwhite,uint32_t idamka){
	int arank = _popc(busy);
	if(arank==0 || arank>8) error("Error ask");
	uint32_t  idx  = blist_get(busy);
	return twobit_get(known[arank] + ((iwhite<<arank) | idamka) * cnk32[arank]/4, idx);
}
