
#include "sha.h"
#include "getarg.h"

int main(int argc, char ** argv){
	TPACK x;
	T12 y;
	if (TCreate(&x, getarg(1),getarg(2),getarg(3)))
		error("Tcreate");

	y = TUnpack(x);

	printf("%08X %X %X -> %08X %08X %08X\n", x.b, x.w, x.d, y.w, y.b, y.d);

	x = TPack(y);

	printf("%08X %X %X\n", x.b, x.w, x.d);
	return 0;
}
