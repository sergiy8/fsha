PROCTYPE int FaceControl(TPACK pos) {
        int small = _popc(pos.b&0x0000000f);
        int big   = _popc(pos.b&0xf0000000);
		int arank = _popc(pos.b);
// least small bits
        if( ((~pos.w)&(~pos.d)) & ((1<<small)-1) ) return -1;
// last big bits
        if( ((pos.w)&(~pos.d)) >> ( arank - big) ) return -1;
        return 0;
}
