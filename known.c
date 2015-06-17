#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "sha.h"

#define MIN_VERSION_WITH_KNO 0x101

static char * dflt[] = {
"192.168.1.2",
"192.168.1.4",
"192.168.1.52",
"192.168.13.111",
};

int main(int argc, char ** argv){
	argc--;
	argv++;
	if(argc==0) {
		argc = sizeof(dflt)/ sizeof(dflt[0]);
		argv = dflt;
	}
	for(int i=0; i< argc; i++) {
		struct sockaddr_in servaddr = {
			.sin_family = AF_INET,
			.sin_addr.s_addr=inet_addr(argv[i]),
			.sin_port = htons(REMOTE_PORT),
		};
		int sha_fd = socket(AF_INET,SOCK_STREAM,0);
		if ( connect(sha_fd, (struct sockaddr*)&servaddr,sizeof(servaddr))){
			printf("%s:connect():%m\n",argv[i]);
			continue;
		}
		if(write(sha_fd,&(struct sha_req){.cmd=htonl(SHA_VER)},sizeof(struct sha_req)) != sizeof(struct sha_req)){
			printf("%s:write():%m\n",argv[i]);
			continue;
		}
		struct sha_resp resp;
		if(read(sha_fd, &resp, sizeof(resp)) != sizeof(resp)){
			printf("%s:read():%m\n",argv[i]);
			continue;
		}
		uint32_t version = ntohl(resp.version);
		if( version < MIN_VERSION_WITH_KNO){
			printf("%s\t%X.%X\n",argv[i],version>>8,version&0xff);
			continue;
		}
		if(write(sha_fd,&(struct sha_req){.cmd=htonl(SHA_KNO)},sizeof(struct sha_req)) != sizeof(struct sha_req)){
			printf("%s:write(SHA_KNO):%m\n",argv[i]);
			continue;
		}
		if(read(sha_fd, &resp, sizeof(resp)) != sizeof(resp)){
			printf("%s:read(SHA_KNO):%m\n",argv[i]);
			continue;
		}
		resp.known = ntohl(resp.known);
		printf("%s\t%X.%X\t",argv[i],version>>8,version&0xff);
		for(int j=0; j< 32; j++)
			if(resp.known & (1<<j))
				printf("%2d",j);
		printf("\n");
	}
	return 0;
}
