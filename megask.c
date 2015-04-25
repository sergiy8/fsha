static inline int megask_bitcontrol(uint32_t b, uint32_t w, uint32_t d) {
	int arank = _popc(b);
	if( 32 - __builtin_clz(w) > arank)
		return -1;
	if( 32 - __builtin_clz(d) > arank)
		return -1;
	return 0;
}
#if MEGASK_REMOTE
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
static int sha_fd;
static void megask_init(void) {
struct sockaddr_in servaddr = {
	.sin_family = AF_INET,
	.sin_addr.s_addr=inet_addr(REMOTE_HOST),
	.sin_port = htons(REMOTE_PORT),
};
	sha_fd = socket(AF_INET,SOCK_STREAM,0);
	if ( connect(sha_fd, (struct sockaddr*)&servaddr,sizeof(servaddr)))
		error("Cannot connect %s:%d",REMOTE_HOST, REMOTE_PORT);
}
static int megask(uint32_t busy, uint32_t iwhite,uint32_t idamka) {
	struct sha_req req = {
		.cmd = htole32(SHA_BXD),
		.b = htole32(busy),
		.w = htole32(iwhite),
		.d = htole32(idamka),
	};
	struct sha_resp resp;
	if(write(sha_fd,&req,sizeof(req)) != sizeof(req))
		return 5;
	if(read(sha_fd, &resp, sizeof(resp)) != sizeof(resp))
		return 5;
	return le32toh(resp.value);
}
#else
#include "malloc_file.c"

static unsigned char * known [32];

static void megask_init(void) {
	int i;
	char fname[PATH_MAX];

	blist_init();

	for(i=0;i<10;i++) {
		struct stat buf;
		snprintf(fname,sizeof(fname),DATA_FORMAT,i);
		if(stat(fname,&buf))
			continue;
		known[i] = (unsigned char*)malloc_file(ARRAY_SIZE_S(i),FMODE_RO,DATA_FORMAT,i);
	}
}

static int megask(uint32_t busy, uint32_t iwhite,uint32_t idamka) __attribute((const));
static int megask(uint32_t busy, uint32_t iwhite,uint32_t idamka) {
	int arank = __builtin_popcount(busy);
	if(known[arank]==NULL)
		return 4;
	if (megask_bitcontrol(busy,iwhite,idamka))
		return 6;
    uint32_t  idx  = blist_get(busy);
    return twobit_get(known[arank] + (uint64_t)((iwhite<<arank) | idamka) * cnk(32,arank)/4, idx);
}
#endif
