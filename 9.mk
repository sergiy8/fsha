NODAMKA:=1
_all:all
WRANK_LIST :=
INCS += megask9.c
UTILS2 += tsorter merger qprocessor dump


step0:
	bin/mk_data${RANK}
	bin/before${RANK}
	bin/klini${RANK}

step:

