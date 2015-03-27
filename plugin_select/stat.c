#ifdef INCLUDE_FOR_TABLE
{"stat",stat_cmd,.helpline = "Current stat from db"},
#else
static void stat_cmd(const char * arg);
#ifndef INCLUDE_FOR_FORWARD


static void stat_cmd(const char * arg){
	printf("%s\n",arg);
}


#endif
#endif
