#include <ctype.h>
#include <locale.h>
#include <ncurses.h>

#include "sha.h"
#include "pack.h"

static WINDOW * wdoska, *whex, *wlog;
uint32_t b = 0xfff00fff;
uint32_t w = 0x00000fff;
uint32_t d = 0;

#define DOSKAY ((LINES-8)/2)
#define DOSKAX 4

#define HEXY (DOSKAY+8)
#define HEXX (DOSKAX + 14)

#define LOGY (HEXY+2)
#define LOGX 0

static void doska(uint32_t w, uint32_t b, uint32_t d){
	int i,j;
	char pole[32]={};
	while(b) {
		int i = __builtin_ffs(b) - 1;
        pole[i] = w&1? 'w' : 'b';
        if(d&1) pole[i] ^= 0x20;
		w/=2;
		d/=2;
		b&=b-1;
    }
	for(i=0;i<8;i++)
	for(j=0;j<4;j++) {
		int k = (28-4*i) + j;
		mvwprintw(wdoska,i, j*3 + 1 + (i&1?0:2),"%c", pole[k]?:'.');
	}
	wrefresh(wdoska);
}

#define putlog(fmt,args...) do{wprintw(wlog,fmt"\n",##args); wrefresh(wlog);}while(0)

int main(){
	setlocale(LC_ALL,"");
	initscr();
	mousemask(ALL_MOUSE_EVENTS, NULL);
	cbreak();

	wdoska = newwin(8,13,DOSKAY,DOSKAX);
	keypad(wdoska,TRUE);

	whex = newwin(1,COLS-HEXX,HEXY,HEXX);
	keypad(whex,TRUE);
//	idcok(whex,TRUE);
//	immedok(whex,TRUE);

	wlog = newwin(LINES-LOGY,COLS,LOGY,LOGX);
	scrollok(wlog,TRUE);
	idlok(wlog,TRUE);

new_doska:
	doska(w,b,d);

	char str[COLS];
	int pos=0;
	snprintf(str,sizeof(str),"%08X %X %X",b,w,d);
	for(;;) {
		MEVENT event;
		mvwprintw(whex,0,0,"%s",str);
		wrefresh(whex);
		int c = mvwgetch(whex,0,pos);
		putlog("%x",c);
		switch(c){
			default:
				if( c!= ' ' && !isxdigit(c))
					continue;
				str[pos++] = c;
				str[pos]=0;
				continue;
			case 0x107:
				if(pos) pos--;
				str[pos]=0;
				continue;
			case 0x0a:
				wclrtobot(whex);
				putlog("=%s=",str);
				pos=0;
				{ uint32_t b2,w2,d2;
				  if(sscanf(str,"%x%x%x",&b2,&w2,&d2)!=3) {
						putlog("Syntax:%s",str);
						continue;
				  }
				  b = b2;
				  w = w2;
				  d = d2;
				  putlog("%08X %X %X",b,w,d);
				  goto new_doska;
				}
			case KEY_MOUSE:
				if(getmouse(&event) != OK) continue;
				putlog("bstate=%x",event.bstate);
				if(!(event.bstate & NCURSES_BUTTON_CLICKED)) continue;
				if (!wmouse_trafo(wdoska, &event.y, &event.x, FALSE)) continue;
				putlog("mouse %d %d",event.y, event.x);
		}
	}
	endwin();
	return 0;
}
