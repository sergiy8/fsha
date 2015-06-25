#include <pthread.h>
#include <stdbool.h>
#include "sha.h"
#include "tcompar.h"
#include "malloc_file.c"

struct input {
	TPACK * pointer;
	char * fname;
	size_t cur,max;
};

static void * sorter( void * arg) {
	struct input * this = arg;
	qsort(this->pointer,this->max,sizeof(TPACK),compar);
	dbg("%s sorted",this->fname);
	return NULL;
}

static int icompar(const void * a, const void * b) {
	const struct input * x = a;
	const struct input * y = b;
	if(y->cur>=y->max)
		return -1;
	if(x->cur>=x->max)
		return 1;
	return compar( x->pointer + x->cur, y->pointer + y->cur);
}

int main(int argc, char ** argv) {
	int i;
	argc--;
	argv++;
	struct input inputs[argc];
	pthread_t pid[argc];
	for(i=0; i<argc; i++) {
		struct stat buf;
		if( stat(argv[i],&buf))
			error("Cannot stat %s:%m",argv[i]);
		if (buf.st_size % sizeof(TPACK))
			error("%s:invalid size",argv[i]);
		inputs[i].pointer = malloc_file(buf.st_size,FMODE_RW,"%s",argv[i]);
		inputs[i].fname = argv[i];
		inputs[i].cur = 0;
		inputs[i].max = buf.st_size / sizeof(TPACK);
		if(pthread_create(pid+i,NULL,sorter, inputs + i))
			error("pthread_create");
	}
	for(i=0; i<argc; i++) {
		pthread_join(pid[i],NULL);
	}
	if( argc == 1) { // Special case, one input stream, just unique copy
		if(inputs[0].max) // paranoid )
			if(fwrite(inputs[0].pointer, sizeof(TPACK), 1, stdout) != 1)
				error("fwrite:%m");
		for(int i=1; i< inputs[0].max; i++)
			if(compar(inputs[0].pointer + i - 1, inputs[0].pointer + i ))
				if(fwrite(inputs[0].pointer + i , sizeof(TPACK), 1, stdout) != 1)
					error("fwrite:%m");
		return 0;
	}
	for(;;) {
		qsort(inputs,argc,sizeof(inputs[0]),icompar);
		TPACK * x = inputs[0].pointer + inputs[0].cur;
		if(fwrite(x, sizeof(TPACK), 1, stdout) != 1)
			error("fwrite:%m");
		for(i=0;i<argc;i++) {
			while( inputs[i].cur < inputs[i].max && compar(inputs[i].pointer + inputs[i].cur, x ) == 0)
				inputs[i].cur ++;
		}
		bool has = false;
		for(i=0;i<argc;i++)
			if(inputs[i].cur < inputs[i].max){
				has = true;
				break;
			}
		if (!has)
			break;
	}
	return 0;
}
