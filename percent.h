static inline void percent_r(uintmax_t a, uintmax_t b, char * res){
        unsigned promile = 1000*a/b;
        sprintf(res,"%2d.%1d%%",promile/10,promile%10);
}
#define percent(a,b) ({ static char loc__[256]; percent_r(a,b,loc__); loc__;})
