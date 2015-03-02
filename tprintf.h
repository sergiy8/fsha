#define tprintf(args...) do{time_t _now=time(NULL)-started; printf("%02u:%02u:%02u ",\
(unsigned)(_now/3600),(unsigned)((_now%3600)/60),(unsigned)(_now%60)); printf(args); }while(0)
