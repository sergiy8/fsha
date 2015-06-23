NODAMKA:=1
_all:all
WRANK_LIST :=
INCS += megask9.c
UTILS2 += tsorter merger qprocessor

9-q1: ${DATADIR}/${RANK}-q1

${DATADIR}/${RANK}-q1.unsorted: ${DATADIR}/${RANK} bin/klini${RANK}
	bin/klini${RANK} -q1
${DATADIR}/${RANK}-q1: ${DATADIR}/${RANK}-q1.unsorted tsorter
	./tsorter $< $@
