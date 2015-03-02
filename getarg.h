
#define getarg(_n) ({\
	uint32_t _x; char * _eptr; int _num=_n;\
	if(_num>=argc) error("argc<%d",_num);\
	_x = strtoul(argv[_num],&_eptr,16);\
	if(*_eptr!=0) error("bad argv[%d]",_num);\
	_x;})
