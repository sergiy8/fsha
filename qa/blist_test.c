#define BLIST_NOFILE 1
#include "sha.h"
#include "getarg.h"

static uint32_t nblist(uint32_t x) {
	uint32_t res = 0;
	uint32_t p;
	int arank;
    for(arank=_popc(x);arank;x^=1<<(31-p),arank--) {
		p = _clz(x);
printf("x=%08X p=%d arank=%d\n",x,p,arank);
		res += cnk(31-p,arank);
	}
	return res;
}

int main(int argc, char ** argv){

 int x = getarg(1);

	blist_init();
	printf("%08X = %d = %d\n", x, blist_get(x), nblist(x));
	return 0;
}
