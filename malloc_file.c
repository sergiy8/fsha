#ifndef _MALLOC_FILE_C
#define _MALLOC_FILE_C
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#define MAP_MODE MAP_SHARED

enum f_mode { FMODE_RO, FMODE_RW, FMODE_CR , FMODE_TMP};

static void * malloc_file(size_t sz, enum f_mode flags , const char * fmt, ...) {
	void * res;
	int f;
	char fname[1024];
	struct stat buf;
	va_list ap;
	va_start (ap,fmt);
	vsnprintf(fname,sizeof(fname),fmt,ap);
	switch(flags) {
	case FMODE_RO:
		f = open(fname,O_RDONLY,0666);
		if(f<0) error("Can't open ro  %s",fname);
		if(fstat(f,&buf)) error("Can't fstat %s",fname);
		if(buf.st_size != sz) error("Illegal size of %s",fname);
		res = mmap (NULL,sz,PROT_READ,MAP_MODE,f,0);
		break;
	case FMODE_TMP:
		umask(0);
		errno=0;
		f = mkstemp(fname);
		if(f<0) error("Can't create %s",fname);
		if(ftruncate(f,sz)) error("Can't truncate %s",fname);
		res = mmap (NULL,sz,PROT_WRITE,MAP_MODE,f,0);
		break;
	case FMODE_CR:
		unlink(fname);
		umask(0);
		errno=0;
		f = open(fname,O_CREAT|O_RDWR,0666);
		if(f<0) error("Can't create %s",fname);
		if(ftruncate(f,sz)) error("Can't truncate %s",fname);
		res = mmap (NULL,sz,PROT_WRITE,MAP_MODE,f,0);
		break;
	case FMODE_RW:
		f = open(fname,O_RDWR);
		if(f<0) error("Can't open rw  %s",fname);
		if(fstat(f,&buf)) error("Can't fstat %s",fname);
		if(buf.st_size != sz) error("Illegal size of %s",fname);
		res = mmap (NULL,sz,PROT_WRITE,MAP_MODE,f,0);
		break;
	default:
		panic();
	}
	close(f);
	if(res==NULL) error("Can't mmap %s",fname);
	return res;
}

//void free_file(void * p, size_t sz){
//	munmap(p,sz);
//}
#endif
