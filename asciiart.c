#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <ncurses.h>

#include "sha.h"
#include "facecontrol.h"
#include "pack.h"
#include "getarg.h"

enum { COLOR_SUCCESS=1, COLOR_FAIL, COLOR_DRAW};

#define remove_bit(x,n)   x = ((x & ~ALLONE((n)+1)) >> 1) | (x & ALLONE(n))
#define insert_bit(x,n,b) x = ((x & ~ALLONE(n))<<1) | (!!(b) << (n)) | (x & ALLONE(n))

static char pens[] = "wbWB";
static int pen = 0;
static char * pens_prompt = "Pen:";

static WINDOW * wdoska, *whex, *wlog, *wask, *whelp, *wpen;
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
#define DOSKASY 8
#define DOSKASX 12

#define HEXY (DOSKAY + DOSKASY - 1)
#define HEXX (DOSKAX + DOSKASX + 8)
#define HEXSY 1
#define HEXSX 20

#define LOGY (HEXY+2)
#define LOGX 0
#define LOGSY (LINES-LOGY)
#define LOGSX COLS

#define ASKY DOSKAY
#define ASKX HEXX
#define ASKSY 3
#define ASKSX 20

#define HELPY 1
#define HELPX 1
#define HELPSY 10
#define HELPSX 40

#define PENY (DOSKAY-2)
#define PENX 1
#define PENSY 1
#define PENSX (__builtin_strlen(pens_prompt) + __builtin_strlen(pens))

#include "megask.c"

static void draw_pen(void){
	int i;
	mvwprintw(wpen,0,0,"%s",pens_prompt);
	for(i=0;pens[i];i++) {
		if(i==pen)
			wattron(wpen,A_REVERSE);
		mvwprintw(wpen,0,i+__builtin_strlen(pens_prompt),"%c",pens[i]);
		if(i==pen)
			wattroff(wpen,A_REVERSE);
	}
	wrefresh(wpen);
}

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
		mvwprintw(wdoska,i, j*3 + (i&1?0:2),"%c", pole[k]?:'.');
	}
	wrefresh(wdoska);
}

const char * summary(void){
	int value = megask(b,w,d);
	int arank = _popc(b);
	if (_popc(b) ==0 ||
		_popc(b) > 24 ||
		_popc(w) > 12 ||
		_popc(w^ALLONE(arank)) > 12
	)
		return "IllegalCount";
	if (FaceControl(b,w,d))
		return "IllegalDamka";
	switch(value) {
		default: return "InternalError";
		case 0:
			if(arank==8) {
				if(_popc(w) != 4)
					return "8-NotInBase";
				else
					return "Draw?";
			} else {
				wattron(wask,COLOR_PAIR(COLOR_YELLOW));
				return "Draw";
			}
		case 1:
			wattron(wask,COLOR_PAIR(COLOR_SUCCESS));
			return "WhiteWin";
		case 2:
			wattron(wask,COLOR_PAIR(COLOR_FAIL));
			return "BlackWin";
		case 3:
			wattron(wask,COLOR_PAIR(COLOR_YELLOW));
			return "DrawAfterTakege";
		case 4:
			return "NotInBase";
		case 5:
			return "CommunicationError";
		case 6:
			return "MegaskBitcontrol";
	}
}

int main(int argc, char ** argv){
	if (argc == 4 ) {
		b = getarg(1);
		w = getarg(2);
		d = getarg(3);
	} else if (argc!=1) {
		error("Three args!");
	}

	megask_init();

	setlocale(LC_ALL,"");
	initscr();

	start_color();
	init_pair(COLOR_SUCCESS,COLOR_GREEN,COLOR_BLACK);
	init_pair(COLOR_FAIL,COLOR_RED,COLOR_BLACK);
	init_pair(COLOR_DRAW,COLOR_YELLOW,COLOR_BLACK);

	mousemask(ALL_MOUSE_EVENTS, NULL);
	cbreak();

	wdoska = newwin(DOSKASY,DOSKASX,DOSKAY,DOSKAX);
	keypad(wdoska,TRUE);

	whex = newwin(HEXSY,HEXSX,HEXY,HEXX);
	keypad(whex,TRUE);
//	idcok(whex,TRUE);
//	immedok(whex,TRUE);

	wlog = newwin(LOGSY,LOGSX,LOGY,LOGX);
	scrollok(wlog,TRUE);
	idlok(wlog,TRUE);

	wask = newwin(ASKSY,ASKSX,ASKY,ASKX);

	whelp = newwin(HELPSY,HELPSX,HELPY,HELPX);
	mvwprintw(whelp,0,0,"TAB - rotate");
	mvwprintw(whelp,1,0,"Shift/Arrows - shift position");
	mvwprintw(whelp,2,0,"Shift/Digit - change pawn");
	wrefresh(whelp);


	wpen = newwin(PENSY,PENSX,PENY,PENX);
	draw_pen();


new_doska:
	doska(w,b,d);

	wattrset(wask,0);
	mvwprintw(wask, 0, 0, "Megask: %d", megask(b,w,d));
	mvwprintw(wask, 1, 0, "Arank : %d-%d", _popc(b),_popc(w));
	mvwprintw(wask, 2, 0, "%s",summary());
	wclrtobot(wask);
	wrefresh(wask);

	char str[HEXSX];
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
						if (invert < _popc(b)) {
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
				  w ^= ALLONE(_popc(b));
				}
				goto new_doska;
			case KEY_MOUSE:
				if(getmouse(&event) != OK) continue;
				if(!(event.bstate & NCURSES_BUTTON_CLICKED)) continue;
				if (wmouse_trafo(wpen, &event.y, &event.x, FALSE)){
					if(event.x < __builtin_strlen(pens_prompt))
						continue;
					pen = event.x - __builtin_strlen(pens_prompt);
					draw_pen();
					continue;
				}
				if (!wmouse_trafo(wdoska, &event.y, &event.x, FALSE)) continue;
				if (event.y & 1)
					event.x+=2;
				if(event.x % 3 != 2)
					continue;
				{   int p = (7 - event.y) * 4 + event.x / 3;
					int num = _popc(b & ALLONE(p));
					putlog("%d num=%d", p, num);
					if( (b & (1<<p)) == 0) { // New
						b |= 1<<p;
						insert_bit(w,num,pen==0 || pen==2);
						insert_bit(d,num,pen>1);
						goto new_doska;
					}
					// remove
					b ^= 1 << p;
					remove_bit(w,num);
					remove_bit(d,num);
					goto new_doska;
				}
				goto new_doska;
		}
	}
	endwin();
	return 0;
}
