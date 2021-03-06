typedef enum {
	ASK_UNK,
	ASK_WHITE,
	ASK_BLACK,
	ASK_DRAW,
	ASK_NODB, // like Http error 404  ;)
	ASK_IO,
} ask_t;
#if MEGASK_REMOTE
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
static int sha_fd;
static uint32_t remote_version;
static void megask_init(void) {
struct sockaddr_in servaddr = {
	.sin_family = AF_INET,
	.sin_addr.s_addr=inet_addr(REMOTE_HOST),
	.sin_port = htons(REMOTE_PORT),
};
	sha_fd = socket(AF_INET,SOCK_STREAM,0);
	if ( connect(sha_fd, (struct sockaddr*)&servaddr,sizeof(servaddr)))
		error("Cannot connect %s:%d",REMOTE_HOST, REMOTE_PORT);
	if(write(sha_fd,&(struct sha_req){.cmd=htonl(SHA_VER)},sizeof(struct sha_req)) != sizeof(struct sha_req))
		error("Cannot ask version from %s:%d",REMOTE_HOST,REMOTE_PORT);
	struct sha_resp resp;
	if(read(sha_fd, &resp, sizeof(resp)) != sizeof(resp))
		error("Ill version reply from %s:%d",REMOTE_HOST, REMOTE_PORT);
	remote_version = ntohl(resp.version);
}
static ask_t megask(TPACK pos) {
	struct sha_req req = {
		.cmd = htonl(SHA_BXD),
		.b = htonl(pos.b),
		.w = htonl(pos.w),
		.d = htonl(pos.d),
	};
	struct sha_resp resp;
	if(write(sha_fd,&req,sizeof(req)) != sizeof(req))
		return ASK_IO;
	if(read(sha_fd, &resp, sizeof(resp)) != sizeof(resp))
		return ASK_IO;
	return ntohl(resp.value);
}

#elif __x86_64__
#include "malloc_file.c"
#include "deepask.c"
#include "megask9.c"

static unsigned char * known [9];

static void megask_init(void) {
	int i;
	char fname[PATH_MAX];

	blist_init();
	struct stat buf;

	for(i=0;i<9;i++) {
		snprintf(fname,sizeof(fname),DATA_FORMAT,i);
		if(stat(fname,&buf))
			continue;
		known[i] = (unsigned char*)malloc_file(ARRAY_SIZE_S(i),FMODE_RO,DATA_FORMAT,i);
	}
	megask9_init();
}

static ask_t megask(TPACK pos) {
	int arank = __builtin_popcount(pos.b);
	switch (arank) {
	default:
		return deepask(pos);
	case 9:
		return megask9(pos);
	case 1 ... 8:
		if(known[arank]==NULL)
			return ASK_NODB;
    	uint32_t  idx  = blist_get(pos.b);
    	return twobit_get(known[arank] + (uint64_t)((pos.w<<arank) | pos.d) * cnk(32,arank)/4, idx) ?: ASK_DRAW;
	}
}
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

static int known[32];
static void megask_init(void) {
	int i;
	char fname[PATH_MAX];

	struct stat buf;

	for(i=0;i<9;i++) {
		snprintf(fname,sizeof(fname),DATA_FORMAT,i);
		if(stat(fname,&buf)){
			fprintf(stderr,"%s:%m \n",fname);
			errno=0;
			continue;
		}
		if (buf.st_size != ARRAY_SIZE_S(i))
			error("Illegal data size %s %lld should be %lld",fname,(long long)buf.st_size, (long long)ARRAY_SIZE_S(i));
		known[i] = open(fname,O_RDONLY);
		if(known[i]<0)
			error("Cannot open RO %s",fname);
	}
}
static ask_t megask(TPACK pos) {
	int arank = __builtin_popcount(pos.b);
	if(known[arank]==0)
		return ASK_NODB;
    uint32_t  idx  = blist_get(pos.b);
	// Dirty - NOT THREAD SAVE!
	if (lseek(known[arank], (off_t)((pos.w<<arank) | pos.d) * cnk(32,arank)/4 + idx /4, SEEK_SET) < 0 )
		error("lseek() arank=%d",arank);
	uint8_t val;
	if (read(known[arank], & val, 1) != 1)
		error("read() arank=%d",arank);
	return twobit_get(&val, idx % 4) ?: ASK_DRAW;
}
#endif
