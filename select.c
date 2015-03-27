#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "sha.h"
#include "itoa/itoa.h"

static char prompt[256];

enum {V_RANK,V_WRANK};
struct {
	const char * name;
	unsigned value;
} vars [] = {
[V_RANK] =  {"rank"},
[V_WRANK] = {"wrank"},
};

#define rank vars[V_RANK].value
#define wrank vars[V_WRANK].value

static void set_prompt(void) {
	char * p = prompt;
	int i;

	p += snprintf(p, p - prompt + sizeof(prompt),"Fsz=%s ", itoa(
wrank? cnk(rank,wrank)*cnk(32,rank)<<(rank-2): cnk(32,rank)<<(2*rank-2)
	));

	if (rank)
		p += snprintf(p, p - prompt + sizeof(prompt),"Jobsz=%s ", itoa(cnk(32,rank)/4));

	for(i=0;i<sizeof(vars)/sizeof(vars[0]);i++)
		if(vars[i].value)
			p += snprintf(p, p - prompt + sizeof(prompt),"%s=%u ",vars[i].name,vars[i].value);
	*p++='>';
	*p++=0;
}

#define eprintf(fmt,args...) printf("Err:"fmt"\n",##args)

static int var_assignment(const char * cmd){
	const char * sep = strchr(cmd,'=');
	if(sep==NULL || sep == cmd)
		return -1;
	int i;
	const char * ename = sep - 1;
	while( ename != cmd && *ename == ' ')
		ename--;
	ename++;
	for(i=0;i<sizeof(vars)/sizeof(vars[0]);i++)
		if(strlen(vars[i].name) == ename - cmd)
			if(strncmp(vars[i].name,cmd, ename -cmd) == 0) {
				char * eptr;
				unsigned long val = strtoul(sep+1,&eptr,0);
				if(*eptr != 0 ) {
					eprintf("Bad value:%s",sep+1);
					return 0;
				}
				vars[i].value = val;
				return 0;
			}
	eprintf("Unknown var:%s",cmd);
	return 0;
}

#define INCLUDE_FOR_FORWARD
#include "select.inc"
#undef INCLUDE_FOR_FORWARD

static struct {
	const char * name;
	void (*func)(const char *);
	const char * helpline;
} cmds [] =  {
#define INCLUDE_FOR_TABLE
#include "select.inc"
#undef INCLUDE_FOR_TABLE
};
#include "select.inc"

static void command(const char * cmd){
	if (var_assignment(cmd)==0)
			return;
	int i;
	if(*cmd=='?') {
		help_cmd(cmd+1);
		return;
	}
	char * sep = strchrnul(cmd,' ');

	for(i=0;i<sizeof(cmds)/sizeof(cmds[0]);i++)
		if(strlen(cmds[i].name) == sep - cmd)
			if(strncmp(cmds[i].name,cmd, sep-cmd) == 0) {
				while(*sep==' ')
					sep++;
				cmds[i].func(sep);
				return;
			}
	eprintf("Unknown cmd:%s",cmd);
}

int main(int argc, char ** argv) {
#ifdef RANK
	rank = RANK;
#endif
#ifdef WRANK
	wrank = WRANK;
#endif
	rl_bind_key('\t',rl_abort); // No file auto-compete
	for(;;) {
		set_prompt();
		char * cmd = readline(prompt);
		if(cmd == NULL)
			break;
		size_t len = strlen(cmd) - 1;
		while( len >= 0 && cmd[len] == ' ')
				len -- ;
		cmd[len+1] = 0;
		if(*cmd==0) {
			free(cmd);
			continue;
		}
		add_history(cmd);
		command(cmd);
	}
	printf("\n");
	return 0;
}
