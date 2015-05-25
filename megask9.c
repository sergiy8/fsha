#include "tcompar.h"
static unsigned char * simple9;
static TPACK *sw, *sb, *sd;
static unsigned sw_count, sb_count, sd_count;

static void megask9_init(void) {
	char fname[PATH_MAX];
	struct stat buf;
	snprintf(fname,sizeof(fname),DATA_FORMAT,9);
	if(stat(fname,&buf))
		return;
	simple9 = malloc_file(cnk(32,9)<<(9-2), FMODE_RO, DATA_FORMAT, 9 );

	if(stat(DATADIR"9-b",&buf))
		error("9-b");
	sb = malloc_file(buf.st_size, FMODE_RO, DATADIR"9-b");
	sb_count = buf.st_size / sizeof(TPACK);

	if(stat(DATADIR"9-w",&buf))
		error("9-w");
	sw = malloc_file(buf.st_size, FMODE_RO, DATADIR"9-w");
	sw_count = buf.st_size / sizeof(TPACK);

	if(stat(DATADIR"9-d",&buf))
		error("9-d");
	sd = malloc_file(buf.st_size, FMODE_RO, DATADIR"9-d");
	sd_count = buf.st_size / sizeof(TPACK);
}

static ask_t megask9(TPACK pos) {
		if (simple9 == NULL)
			return ASK_NODB;
        if(pos.d) {
			if (bsearch(&pos, sw, sw_count, sizeof(TPACK), compar))
				return ASK_WHITE;
			if (bsearch(&pos, sb, sb_count, sizeof(TPACK), compar))
				return ASK_BLACK;
			if (bsearch(&pos, sd, sd_count, sizeof(TPACK), compar))
				return ASK_DRAW;
			return ASK_NODB;
		}
        uint32_t  idx  = blist_get(pos.b);
        return twobit_get(simple9 + (uint64_t)pos.w * cnk(32,9)/4, idx);
}
