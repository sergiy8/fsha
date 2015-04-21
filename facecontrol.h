PROCTYPE int FaceControl(uint32_t busy, uint32_t iwhite, uint32_t idamka) {
        int small = _popc(busy&0x0000000f);
        int big   = _popc(busy&0xf0000000);
		int arank = _popc(busy);
// least small bits
        if( ((~iwhite)&(~idamka)) & ((1<<small)-1) ) return -1;
// last big bits
        if( ((iwhite)&(~idamka)) >> ( arank - big) ) return -1;
        return 0;
}
