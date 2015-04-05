#ifdef INCLUDE_FOR_TABLE
{"vars",vars_cmd,.helpline = "List of available vars"},
#else
static void vars_cmd(const char * arg);
#ifndef INCLUDE_FOR_FORWARD

static void vars_cmd(const char * arg){
	unsigned i;
	for(i=0;i<sizeof(vars)/sizeof(vars[0]);i++)
		printf("%s\t%s\n",vars[i].name,vars[i].helpline?:"");
}

#endif
#endif
