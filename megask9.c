
static unsigned char * simple9;

static void megask9_init(void) {
	char fname[PATH_MAX];
	struct stat buf;
	snprintf(fname,sizeof(fname),DATA_FORMAT,9);
	if(stat(fname,&buf))
		return;
	simple9 = malloc_file(cnk(32,9)<<(9-2), FMODE_RO, DATA_FORMAT, 9 );
}

static int megask9(TPACK pos) {
		if (simple9 == NULL)
			return ASK_NODB;
        if(pos.d)
            return ASK_NODB;
        uint32_t  idx  = blist_get(pos.b);
        return twobit_get(simple9 + (uint64_t)pos.w * cnk(32,arank)/4, idx);
}
