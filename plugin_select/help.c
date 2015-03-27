#ifdef INCLUDE_FOR_TABLE
{"help",help_cmd,.helpline = "List of available commands"},
#else
static void help_cmd(const char * arg);
#ifndef INCLUDE_FOR_FORWARD

static void help_cmd(const char * arg){
   int i;
	if(*arg == 0) {
		for(i=0;i<sizeof(cmds)/sizeof(cmds[0]);i++)
			printf("%s\t%s\n",cmds[i].name,cmds[i].helpline?cmds[i].helpline:"");
		return;
	}
	else
		printf("%s\n",arg); //TODO
}

#endif
#endif
