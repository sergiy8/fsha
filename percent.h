static inline void percent_r(uintmax_t a, uintmax_t b, char * res){
        unsigned promile = (1000*a + b/2)/b;
        sprintf(res,"%2u.%1u%%",promile/10,promile%10);
}
#define percent(a,b) ({ static char loc__[256]; percent_r(a,b,loc__); loc__;})
