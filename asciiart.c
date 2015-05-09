#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <ncurses.h>

#include "sha.h"
#include "facecontrol.h"
#include "pack.h"
#include "getarg.h"

static WINDOW * wdoska, *whex, *wlog, *wask, *whelp, *wpen;
#define putlog(fmt,args...) do{wprintw(wlog,fmt"\n",##args); wrefresh(wlog);}while(0)
#include "megask.c"

enum { COLOR_SUCCESS=1, COLOR_FAIL, COLOR_DRAW};

#define remove_bit(x,n)   x = ((x & ~ALLONE((n)+1)) >> 1) | (x & ALLONE(n))
#define insert_bit(x,n,b) x = ((x & ~ALLONE(n))<<1) | (!!(b) << (n)) | (x & ALLONE(n))

static char pens[] = "xyXYC";
static int pen = 0;
static char * pens_prompt = "Pen:";


static uint32_t b,w,d;
static int rotate = 0;

#define MAXHISTORY 100
static int curhistory=0;
static uint32_t b_h[MAXHISTORY],w_h[MAXHISTORY],d_h[MAXHISTORY],r_h[MAXHISTORY];
static void push_h(void){
	if(curhistory==MAXHISTORY){
		putlog("History overflow, max %d",MAXHISTORY);
		return;
	}
	b_h[curhistory]=b;
	w_h[curhistory]=w;
	d_h[curhistory]=d;
	r_h[curhistory]=rotate;
	curhistory++;
}
static void pop_h(void) {
	if(curhistory==0) {
		putlog("No previous position");
		return;
	}
	curhistory--;
	b = b_h[curhistory];
	w = w_h[curhistory];
	d = d_h[curhistory];
	rotate = r_h[curhistory];
}

#define MAXFUTURE 40
static int maxfuture = 0;
static struct {
	uint32_t b,w,d;
	WINDOW * doska;
} future[MAXFUTURE];

#define HELPY 0
#define HELPX 0
#define HELPSY 10
#define HELPSX 40

#define PENY (HELPY + HELPSY + 1)
#define PENX 1
#define PENSY 1
#define PENSX (__builtin_strlen(pens_prompt) + __builtin_strlen(pens))

#define DOSKAY (PENY + PENSY)
#define DOSKAX 4
#define DOSKASY 8
#define DOSKASX 12

#define ASKY DOSKAY
#define ASKX (DOSKAX + DOSKASX + 3)
#define ASKSY 5
#define ASKSX 20

#define HEXY (DOSKAY + DOSKASY - 1)
#define HEXX ASKX
#define HEXSY 1
#define HEXSX ASKSX

#define LOGY (DOSKAY + DOSKASY + 1)
#define LOGX 0
#define LOGSY (LINES-LOGY)
#define LOGSX 40

#define FUTUREY 0
#define FUTUREX (HEXX + HEXSX + 1)
#define FUTURESY (DOSKASY+2)
#define FUTURESX (DOSKASX+4)


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

static void doska(WINDOW * wdoska, uint32_t b, uint32_t w, uint32_t d){
	int i,j;
	char pole[32]={};
	while(b) {
		int i = __builtin_ffs(b) - 1;
        pole[i] = (w&1)^rotate? 'x' : 'y';
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
			wattron(wask,COLOR_PAIR(COLOR_YELLOW));
			return "Draw";
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

static int revert_ask(int x) {
	if(x==1)
		return 2;
	if(x==2)
		return 1;
	return x;
}

#define MOVE_FIND_ALL 1
int MoveBlack(uint32_t b, uint32_t w, uint32_t d){
	if (maxfuture >= MAXFUTURE ) {
		putlog("Max=%d, Ign wbd=%08X %08X %08X",MAXFUTURE, w, b, d);
		return 0;
	}
	const int perline = (COLS - FUTUREX ) / FUTURESX;
	Pack( &future[maxfuture].b, &future[maxfuture].w, &future[maxfuture].d, (w), (b), (d));
	if (perline) {
		if(future[maxfuture].doska == NULL )
			future[maxfuture].doska = newwin(FUTURESY,FUTURESX,
				FUTUREY + (maxfuture / perline )*FUTURESY,
				FUTUREX + (maxfuture % perline)*FUTURESX);
		WINDOW * this = future[maxfuture].doska;
		uint32_t x,y,z;
		Pack(&x,&y,&z,_brev(b),_brev(w),_brev(d));
		int value = revert_ask(megask(x,y,z));
		if(value == 1)
			wattron(this,COLOR_PAIR(COLOR_SUCCESS));
		else if (value == 2)
			wattron(this,COLOR_PAIR(COLOR_FAIL));
		mvwprintw(this,DOSKASY,0,"%X %X %X=%d", future[maxfuture].b, future[maxfuture].w, future[maxfuture].d, value);
		wattrset(this,0);
		doska(this, future[maxfuture].b, future[maxfuture].w, future[maxfuture].d);
	}
	maxfuture++;
	return 0;
}
#include "move4.c"

int main(int argc, char ** argv){
	if (argc == 4 ) {
		b = getarg(1);
		w = getarg(2);
		d = getarg(3);
	} else if (argc==2) {
		FILE * f = fopen(argv[1],"r");
		if(f==NULL)
			error("Is it file:%s",argv[1]);
		if(fscanf(f,"%x%x%x",&b,&w,&d)!=3)
			error("%s format error",argv[1]);
		fclose(f);
	} else if (argc==1) {
		b = 0x00f00f00;
		w = 0xf;
		d = 0;
	} else {
		error("File name - or Three args!");
	}

	megask_init();

	setlocale(LC_ALL,"");
new_screen:
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
#if MEGASK_REMOTE
	mvwprintw(whelp,3,0,"DB=%s",REMOTE_HOST);
#else
	mvwprintw(whelp,3,0,"DB=%s",DATADIR);
#endif
	wrefresh(whelp);


	wpen = newwin(PENSY,PENSX,PENY,PENX);
	draw_pen();


new_doska:
	doska(wdoska, b,w,d);

	wattrset(wask,0);
	mvwprintw(wask, 0, 0, "Move: %c", rotate?'Y':'X');
	mvwprintw(wask, 1, 0, "Megask: %d", megask(b,w,d));
	mvwprintw(wask, 2, 0, "Arank : %d-%d", _popc(b),_popc(w));
	mvwprintw(wask, 3, 0, "%s",summary());
	wclrtoeol(wask);
	 wattrset(wask,0);
	mvwprintw(wask, 4, 0, "%d",curhistory);
	wclrtobot(wask);
	wrefresh(wask);

	{ uint32_t x,y,z;
	  for(x=0;x<maxfuture;x++){
		werase(future[x].doska);
	    wrefresh(future[x].doska);
	  }
	  maxfuture=0;
	  Unpack(b,w,d,&x,&y,&z);
	  //putlog("%08X %X %X unpacked =  %08X %08X %08X",b,w,d,x,y,z);
	  MoveWhite(x,y,z);
	}

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
							push_h();
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
			case KEY_LEFT:
				if(pos) pos--;
				continue;
			case KEY_RIGHT:
				if(str[pos]==0)
					continue;
				pos++;
				continue;
			case 0x0a:
				wclrtobot(whex);
				pos=0;
				{ uint32_t b2,w2,d2,e2;
				  if(sscanf(str,"%x%x%x%x",&b2,&w2,&d2,&e2)!=3) {
						putlog("Syntax:%s",str);
						continue;
				  }
				push_h();
				  b = b2;
				  w = w2;
				  d = d2;
				  goto new_doska;
				}
			case KEY_SF: // shift - down
				if (b&0xf) continue;
				push_h();
				b>>=4;
				goto new_doska;
			case KEY_SR: // shift-up
				if (b&0xf0000000) continue;
				push_h();
				b<<=4;
				goto new_doska;
			case KEY_SRIGHT: //shift-right
				if( b&0x88888888) continue;
				push_h();
				b<<=1;
				goto new_doska;
			case KEY_SLEFT: //shift-left
				if(b&0x11111111) continue;
				push_h();
				b>>=1;
				goto new_doska;
			case 9:
				push_h();
				do_rotate:
				{ uint32_t ub,uw,ud;
				  Unpack(b,w,d,&uw,&ub,&ud);
				  uw = _brev(uw);
				  ub = _brev(ub);
				  ud = _brev(ud);
				  Pack(&b,&w,&d,ub,uw,ud);
				}
				rotate^=1;
				goto new_doska;
			case KEY_MOUSE:
				if(getmouse(&event) != OK) continue;
				if(!(event.bstate & NCURSES_BUTTON_CLICKED)) continue;
				//PEN
				if (wmouse_trafo(wpen, &event.y, &event.x, FALSE)){
					if(event.x < __builtin_strlen(pens_prompt))
						continue;
					int new_pen = event.x - __builtin_strlen(pens_prompt);
					switch(pens[new_pen]) {
						default: break;
						case 'C':
							push_h();
							b = w = d = 0;
							rotate = 0;
							goto new_doska;
					}
					pen = new_pen;
					draw_pen();
					continue;
				}
				// FUTURE
				{ int x;
					for(x=0;x<maxfuture;x++)
					if(wmouse_trafo(future[x].doska, &event.y, &event.x, FALSE)) {
						push_h();
						b = future[x].b;
						w = future[x].w;
						d = future[x].d;
						goto do_rotate;
					}
				}
				//DOSKA
				if (!wmouse_trafo(wdoska, &event.y, &event.x, FALSE)) continue;
				if (event.y & 1)
					event.x+=2;
				if(event.x % 3 != 2)
					continue;
				{   int p = (7 - event.y) * 4 + event.x / 3;
					int num = _popc(b & ALLONE(p));
					if( (b & (1<<p)) == 0) { // New
						push_h();
						b |= 1<<p;
						insert_bit(w,num,rotate ^ !!(pen==0 || pen==2));
						insert_bit(d,num,pen>1);
						goto new_doska;
					}
					// remove
					push_h();
					b ^= 1 << p;
					remove_bit(w,num);
					remove_bit(d,num);
					goto new_doska;
				}
				goto new_doska;
			case 0x107:
				pop_h();
				goto new_doska;
			case KEY_RESIZE:
				if(COLS <= FUTUREX || LINES <= LOGY )
					continue;
				for(;maxfuture;maxfuture--) {
					werase(future[maxfuture].doska);
					wrefresh(future[maxfuture].doska);
					delwin(future[maxfuture].doska);
					future[maxfuture].doska = 0;
				}
				endwin();
				goto new_screen;
		}
	}
	endwin();
	return 0;
}
