#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#undef RANK
#define RANK 24
#include "sha.h"

#undef MEGASK_REMOTE
#include "megask.c"

static void * dialogue(void * arg) {
	struct sha_req req;
	struct sha_resp resp;
	int fd = (uintptr_t)arg;
	for(;;) {
		if(read(fd,&req,sizeof(req)) != sizeof(req))
			break;
		switch(le32toh(req.cmd)) {
		default:
			resp.value = -1;
			break;
		case SHA_BXD:
			resp.value = megask(le32toh(req.b), le32toh(req.w), le32toh(req.d));
			break;
		case SHA_BLIST:
			resp.value = blist_get(le32toh(req.b));
			break;
		}
		resp.value = htole32(resp.value);
		if (write(fd,&resp,sizeof(resp)) != sizeof(resp))
			break;
	}
	close(fd);
	return NULL;
}

int main() {

	megask_init();

	struct sockaddr_in servaddr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr=htonl(INADDR_ANY),
		.sin_port=htons(REMOTE_PORT),
	};
	int mainsock = socket(AF_INET,SOCK_STREAM,0);
	if (bind(mainsock,(struct sockaddr *)&servaddr,sizeof(servaddr)) || listen(mainsock,1024))
		error("Cannot bind socket");

	if ( daemon(0,0) )
		error("Cannot daemonize");

	for(;;) {
		struct sockaddr_in client;
		socklen_t clilen = sizeof(client);
		int connfd = accept(mainsock,(struct sockaddr*)&client,&clilen);
		if (connfd < 0)
			error("Cannot accept");
		pthread_t pth;
		if(pthread_create(&pth,NULL,dialogue,(void*)(uintptr_t)connfd))
			error("pthread_create");
		pthread_detach(pth);
	}
}
