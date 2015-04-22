#define REMOTE_PORT 51234
#define REMOTE_HOST "192.168.13.111"

#include <endian.h>
// Little Endian!

struct sha_req {
	uint32_t b;
	uint32_t w;
	uint32_t d;
};

struct sha_resp {
	uint32_t value;
};
