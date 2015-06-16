#define REMOTE_PORT 51234
//#define REMOTE_HOST "192.168.13.111"
#define REMOTE_HOST "192.168.1.2"

#define REMOTE_PROTOCOL_VERSION 0x100

// Network endian !

typedef enum {
		SHA_BXD='bxd',
		SHA_VER='sVer',
		SHA_KNO='know',
} remote_command_t;

struct sha_req {
	uint32_t cmd;
	union {
		struct {
			uint32_t b;
			uint32_t w;
			uint32_t d;
		};
		uint32_t version;
		uint32_t known;
	};
};

struct sha_resp {
	union {
		uint32_t value;
		uint32_t version;
		uint32_t known;
	};
};
