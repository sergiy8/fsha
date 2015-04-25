#define REMOTE_PORT 51234
#define REMOTE_HOST "192.168.13.111"

#include <endian.h>
// Little Endian!

typedef enum {SHA_BXD='bxd', SHA_BLIST='blst'} remote_command_t;

struct sha_req {
	uint32_t cmd;
	uint32_t b;
	uint32_t w;
	uint32_t d;
};

struct sha_resp {
	uint32_t value;
};
