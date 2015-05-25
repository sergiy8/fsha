NODAMKA:=1
_all:all
WRANK_LIST :=
UTILS2 += tsorter p_before p_klini

9-q1: ${DATADIR}/${RANK}-q1

${DATADIR}/${RANK}-q1.unsorted: ${DATADIR}/${RANK} bin/klini${RANK}
	bin/klini${RANK} -q1
${DATADIR}/${RANK}-q1: ${DATADIR}/${RANK}-q1.unsorted tsorter
	./tsorter $< $@

#bin/klini9: CC += -DKLINI_MEGASK=1
