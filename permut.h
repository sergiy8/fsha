#ifndef _PERMUT_H
#define _PERMUT_H
#if 0
static inline uint32_t _permut(uint32_t v){
        uint32_t  t = v | (v - 1); // t gets v's least significant 0 bits set to 1
// Next set to 1 the most significant bit to change, 
// set to 0 the least significant ones, and add the necessary 1 bits.
        return (t + 1) | (((~t & -~t) - 1) >> (__builtin_ctz(v) + 1));
}
#endif

#if 1
// this version return 0xffffffff after the last permutation
static inline uint32_t _permut(uint32_t v){
        uint32_t  t = (v | (v - 1)) + 1;  
	return  t | ((((t & -t) / (v & -v)) >> 1) - 1);  
}
#endif
#endif
