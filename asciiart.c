#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <ncurses.h>

#include "sha.h"
#include "pack.h"


static WINDOW * wdoska, *whex, *wlog, *wask, *whelp;
#define putlog(fmt,args...) do{wprintw(wlog,fmt"\n",##args); wrefresh(wlog);}while(0)

#if 0
uint32_t b = 0xfff00fff;
uint32_t w = 0x00000fff;
#else
uint32_t b = 0x00f00f00;
uint32_t w = 0x0000000f;
#endif
uint32_t d = 0;

#define DOSKAY ((LINES-8)/2)
#define DOSKAX 4

#define HEXY (DOSKAY+8)
#define HEXX (DOSKAX + 14)

#define LOGY (HEXY+2)
#define LOGX 0

#define ASKY DOSKAY
#define ASKX (DOSKAX+20)

#define HELPY 1
#define HELPX 1

#include "megask.inc"

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

	wlog = newwin(LINES-LOGY,COLS-LOGX,LOGY,LOGX);
	scrollok(wlog,TRUE);
	idlok(wlog,TRUE);

	wask = newwin(2,20,ASKY,ASKX);

	whelp = newwin(5,COLS-HELPX,HELPY,HELPX);
	mvwprintw(whelp,0,0,"TAB - rotate");
	mvwprintw(whelp,1,0,"Shift/Arrows - shift position");
	mvwprintw(whelp,2,0,"Shift/Digit - change pawn");
	wrefresh(whelp);

	megask_init();

new_doska:
	doska(w,b,d);

	mvwprintw(wask, 0, 0, "Megask: %d", megask(b,w,d));
	wrefresh(wask);

	char str[COLS];
	int pos=0;
	snprintf(str,sizeof(str),"%08X %X %X          ",b,w,d);
	for(;;) {
		MEVENT event;
		mvwprintw(whex,0,0,"%s",str);
		wrefresh(whex);
		int c = mvwgetch(whex,0,pos);
		switch(c){
			default:
				{ const char * klava="!@#$%^&*()_";
				  char * p = strchr(klava,c);
				  if(p!=NULL) {
						int invert = p - klava;
						if (invert < __builtin_popcount(b)) {
							w ^= 1<<invert;
							goto new_doska;
						}
				  }
				}
				if( c!= ' ' && !isxdigit(c)) {
					putlog("%x",c);
					continue;
				}
				if(str[pos]==0)
					continue;
				str[pos++] = c;
				continue;
			case 0x104: // Left
				if(pos) pos--;
				continue; // Right
			case 0x105:
				if(str[pos]==0)
					continue;
				pos++;
				continue;
			case 0x107: // Del
				if(pos==0)
					continue;
				pos--;
				{ int i;
				  for(i=pos;str[i+1];i++)
					str[i]=str[i+1];
				  str[i]=0;
				}
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
			case 0x150: // shift - down
				if (b&0xf) continue;
				b>>=4;
				goto new_doska;
			case 0x151: // shift-up
				if (b&0xf0000000) continue;
				b<<=4;
				goto new_doska;
			case 0x192: //shift-right
				if( b&0x88888888) continue;
				b<<=1;
				goto new_doska;
			case 0x189: //shift-left
				if(b&0x11111111) continue;
				b>>=1;
				goto new_doska;
			case 9:
				{ uint32_t ub,uw,ud;
				  Unpack(b,w,d,&uw,&ub,&ud);
				  uw = _brev(uw);
				  ub = _brev(ub);
				  ud = _brev(ud);
				  Pack(&b,&w,&d,uw,ub,ud);
				  w ^= ALLONE(__builtin_popcount(b));
				}
				goto new_doska;
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
