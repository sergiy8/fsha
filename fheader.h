#define FHEADER_MAGIC 0x10
struct fheader {
	unsigned magic:8;
	unsigned rank:5;
	unsigned value:2;
	unsigned wbit:1;
	unsigned sorted:1;
	unsigned full:1;

	unsigned w:24;
	unsigned d:24;

	uint32_t count0;
	uint32_t count1;
	uint32_t data[];
}; ///4*32 bits

#define DHEADER_MAGIC 0x20
#define DB_FORMAT DATADIR"%d.db"
struct dheader {
	uint64_t magic;
	uint64_t maxo;
	uint64_t offsets[];
};
