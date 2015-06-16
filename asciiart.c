#include <ctype.h>
#include <string.h>
#include <locale.h>
#include <ncurses.h>

#include "sha.h"
#include "facecontrol.h"
#include "getarg.h"

static WINDOW * wdoska, *whex, *wlog, *wask, *whelp, *wpen;
#define putlog(fmt,args...) do{wprintw(wlog,fmt"\n",##args); wrefresh(wlog);}while(0)
#include "megask.c"

enum { COLOR_SUCCESS=1, COLOR_FAIL, COLOR_DRAW, COLOR_NODB};

#define remove_bit(x,n)   x = ((x & ~ALLONE((n)+1)) >> 1) | (x & ALLONE(n))
#define insert_bit(x,n,b) x = ((x & ~ALLONE(n))<<1) | (!!(b) << (n)) | (x & ALLONE(n))

static char pens[] = "xyXYC";
static int pen = 0;
static char * pens_prompt = "Pen:";


static TPACK cp; // Current position
static int rotate = 0;

#define MAXHISTORY 100
static int curhistory=0;
static TPACK t_h[MAXHISTORY];
static int r_h[MAXHISTORY];
static void push_h(void){
	if(curhistory==MAXHISTORY){
		putlog("History overflow, max %d",MAXHISTORY);
		return;
	}
	t_h[curhistory] = cp;
	r_h[curhistory]=rotate;
	curhistory++;
}
static void pop_h(void) {
	if(curhistory==0) {
		putlog("No previous position");
		return;
	}
	curhistory--;
	cp = t_h[curhistory];
	rotate = r_h[curhistory];
}

#define MAXFUTURE 40
static int maxfuture = 0;
static struct {
	TPACK cp;
	int value;
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
#define ASKSY 6
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
#define FUTURESX (DOSKASX+5)

static const char rank_name[32] = "0123456789ABCDEFGHIJKLMNOPQRST";

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

static void doska(WINDOW * wdoska, TPACK cp){
	int i,j;
	char pole[32]={};
	while(cp.b) {
		int i = __builtin_ffs(cp.b) - 1;
        pole[i] = (cp.w&1)^rotate? 'x' : 'y';
        if(cp.d&1) pole[i] ^= 0x20;
		cp.w/=2;
		cp.d/=2;
		cp.b&=cp.b-1;
    }
	for(i=0;i<8;i++)
	for(j=0;j<4;j++) {
		int k = (28-4*i) + j;
		mvwprintw(wdoska,i, j*3 + (i&1?0:2),"%c", pole[k]?:'.');
	}
	wrefresh(wdoska);
}

const char * summary(int value){
	int arank = _popc(cp.b);
	if (_popc(cp.b) ==0 ||
		_popc(cp.b) > 24 ||
		_popc(cp.w) > 12 ||
		_popc(cp.w^ALLONE(arank)) > 12
	)
		return "IllegalCount";
	if (FaceControl(cp))
		return "IllegalDamka";
	if ( IsForced(cp))
		wattron(wask,A_REVERSE);
	switch(value) {
		default: return "InternalError";
		case ASK_BAD:
		case ASK_DRAW:
			wattron(wask,COLOR_PAIR(COLOR_DRAW));
			return "Draw";
		case ASK_WHITE:
			wattron(wask,COLOR_PAIR(COLOR_SUCCESS));
			return "WhiteWin";
		case ASK_BLACK:
			wattron(wask,COLOR_PAIR(COLOR_FAIL));
			return "BlackWin";
		case ASK_NODB:
			wattron(wask,COLOR_PAIR(COLOR_NODB));
			return "NotInBase";
	}
}

static int revert_ask(int x) {
	if(x==1)
		return 2;
	if(x==2)
		return 1;
	return x;
}

static inline T12 swap_t12(T12 x) {
	return (T12){x.b,x.w,x.d};
}

#define MOVE_FIND_ALL 1
int MoveBlack(T12 pos){
	if (maxfuture >= MAXFUTURE ) {
		putlog("Max=%d, Ign wbd=%08X %08X %08X",MAXFUTURE, pos.w, pos.b, pos.d);
		return 0;
	}
	future[maxfuture].cp = TPack(swap_t12(pos));
	future[maxfuture].value = revert_ask(megask(TPack((T12){_brev(pos.w),_brev(pos.b),_brev(pos.d)})));
	maxfuture++;
	return 1;
}
#include "move4.c"
void EraseFuture(void){
	for(int x=0;x<maxfuture;x++){
		werase(future[x].doska);
		wrefresh(future[x].doska);
		delwin(future[x].doska);
		future[x].doska = 0;
	}
	maxfuture=0;
}
void DrawFuture(void){
	const int perline = (COLS - FUTUREX ) / FUTURESX;
	if (!perline)
		return;
	int f;
	for(f=0;f< maxfuture;f++){
		if(future[f].doska) {
			werase(future[f].doska);
			wrefresh(future[f].doska);
			delwin(future[f].doska);
			future[f].doska = 0;
		}
		if ( FUTUREY + ( f/perline + 1) * FUTURESY > LINES ) {
			putlog("Small window, can' draw %d", f);
			continue;
		}
		future[f].doska = newwin(FUTURESY,FUTURESX,
				FUTUREY + (f / perline )*FUTURESY,
				FUTUREX + (f % perline)*FUTURESX);
		WINDOW * this = future[f].doska;
		if(future[f].value == ASK_WHITE)
			wattron(this,COLOR_PAIR(COLOR_SUCCESS));
		else if (future[f].value == ASK_BLACK)
			wattron(this,COLOR_PAIR(COLOR_FAIL));
		else if (future[f].value == ASK_NODB)
			wattron(this,COLOR_PAIR(COLOR_NODB));
		switch (IsForced(TRotate(future[f].cp))){
		case -2:
			wattron(this,A_BOLD);
			wattron(this,A_REVERSE);
			break;
		case -1:
			wattron(this,A_REVERSE);
			break;
		default:;
		}
		mvwprintw(this,DOSKASY,0,"%X %X %X=%d", future[f].cp.b, future[f].cp.w, future[f].cp.d, future[f].value);
		wattrset(this,0);
		doska(this, future[f].cp);
	}
}

int main(int argc, char ** argv){
	if (argc == 5 ) { // Extra value from klini log
		int x = getarg(4);
		if ( x == 1 || x == 2 )
			argc=4;
	}
	if (argc == 4 ) {
		if( TCreate(&cp,getarg(1),getarg(2),getarg(3)))
			error("Tcreate");
	} else if (argc==2) {
		unsigned x,y,z;
		FILE * f = fopen(argv[1],"r");
		if(f==NULL)
			error("Is it file:%s",argv[1]);
		if(fscanf(f,"%x%x%x",&x,&y,&z)!=3)
			error("%s format error",argv[1]);
		fclose(f);
		if( TCreate(&cp,x,y,z))
			error("Tcreate");
	} else if (argc==1) {
		if( TCreate(&cp,0x00f00f00,0xf,0))
			error("Tcreate");
	} else {
		error("File name - or Three args!");
	}

	megask_init();

	setlocale(LC_ALL,"");
	initscr();

	start_color();
	init_pair(COLOR_SUCCESS,COLOR_GREEN,COLOR_BLACK);
	init_pair(COLOR_FAIL,COLOR_RED,COLOR_BLACK);
	init_pair(COLOR_DRAW,COLOR_WHITE,COLOR_BLACK);
	init_pair(COLOR_NODB,COLOR_BLUE,COLOR_BLACK);

	mousemask(ALL_MOUSE_EVENTS, NULL);
	cbreak();

	wdoska = newwin(DOSKASY,DOSKASX,DOSKAY,DOSKAX);
	keypad(wdoska,TRUE);

	whex = newwin(HEXSY,HEXSX,HEXY,HEXX);
	keypad(whex,TRUE);

	wlog = newwin(LOGSY,LOGSX,LOGY,LOGX);
	scrollok(wlog,TRUE);
	idlok(wlog,TRUE);

	wask = newwin(ASKSY,ASKSX,ASKY,ASKX);

	whelp = newwin(HELPSY,HELPSX,HELPY,HELPX);
	mvwprintw(whelp,0,0,"TAB - rotate");
	mvwprintw(whelp,1,0,"Shift/Arrows - shift position");
	mvwprintw(whelp,2,0,"Shift/Digit - change pawn");
#if MEGASK_REMOTE
	mvwprintw(whelp,3,0,"DB=%s v.%X",REMOTE_HOST,remote_version);
#else
	mvwprintw(whelp,3,0,"DB=%s",DATADIR);
#endif
	wrefresh(whelp);


	wpen = newwin(PENSY,PENSX,PENY,PENX);
	draw_pen();


new_doska:
	doska(wdoska, cp);

	int value = megask(cp);
	wattrset(wask,0);
	mvwprintw(wask, 0, 0, "Move: %c", rotate?'Y':'X');
	mvwprintw(wask, 1, 0, "Megask: %d", value);
	mvwprintw(wask, 2, 0, "Arank : %c-%c", rank_name[_popc(cp.b)],rank_name[_popc(cp.w)]);
	mvwprintw(wask, 3, 0, "%s",summary(value));
	wclrtoeol(wask);
	 wattrset(wask,0);
	mvwprintw(wask, 4, 0, "%d",curhistory);
//	mvwprintw(wask, 5, 0, "%s",IsForced(cp)?"Forced":"");
	wclrtobot(wask);
	wrefresh(wask);

	EraseFuture();
	MoveWhite(TUnpack(cp));
new_screen:
	DrawFuture();

	char str[HEXSX];
	int pos=0;
	snprintf(str,sizeof(str),"%08X %X %X          ",cp.b,cp.w,cp.d);
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
						if (invert < _popc(cp.b)) {
							push_h();
							cp.w ^= 1<<invert;
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
				if (TCreate(&cp,b2,w2,d2)) {
					pop_h();
					putlog("Tcreate error");
					continue;
				}
				  goto new_doska;
				}
			case KEY_SF: // shift - down
				if (cp.b&0xf) continue;
				push_h();
				cp.b>>=4;
				goto new_doska;
			case KEY_SR: // shift-up
				if (cp.b&0xf0000000) continue;
				push_h();
				cp.b<<=4;
				goto new_doska;
			case KEY_SRIGHT: //shift-right
				if( cp.b&0x88888888) continue;
				push_h();
				cp.b<<=1;
				goto new_doska;
			case KEY_SLEFT: //shift-left
				if(cp.b&0x11111111) continue;
				push_h();
				cp.b>>=1;
				goto new_doska;
			case 9:
				push_h();
				do_rotate:
				cp = TRotate(cp);
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
							cp.b = cp.w = cp.d = 0;
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
						cp = future[x].cp;
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
					int num = _popc(cp.b & ALLONE(p));
					if( (cp.b & (1<<p)) == 0) { // New
						push_h();
						cp.b |= 1<<p;
						insert_bit(cp.w,num,rotate ^ !!(pen==0 || pen==2));
						insert_bit(cp.d,num,pen>1);
						goto new_doska;
					}
					// remove
					push_h();
					cp.b ^= 1 << p;
					remove_bit(cp.w,num);
					remove_bit(cp.d,num);
					goto new_doska;
				}
				goto new_doska;
			case 0x107:
				pop_h();
				goto new_doska;
			case KEY_RESIZE:
				if(COLS <= FUTUREX || LINES <= LOGY )
					continue;
				goto new_screen;
		}
	}
	endwin();
	return 0;
}
