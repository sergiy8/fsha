
#include "sha.h"
#include "pack.h"
#include "getarg.h"

static uint32_t busy,iwhite,idamka,w,b,d;

static void p(const char * s){
	printf("busy=%08X iwhite=%X idamka=%X w=%08X b=%08X d=%08X %s\n",busy,iwhite,idamka,w,b,d,s);
}

int main(int argc, char ** argv){
	w = getarg(1);
	b = getarg(2);
	d = getarg(3);
	p("inited");
	Pack(&busy,&iwhite,&idamka,w,b,d);
	p("pucked");
	w=0, b=0, d=0;
	Unpack(busy,iwhite,idamka,&w,&b,&d);
	p("Unpucked");
	return 0;
}
