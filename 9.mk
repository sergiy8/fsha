NODAMKA:=1
_all:all
WRANK_LIST :=
INCS += megask9.c deepask.c
UTILS2 += merger qprocessor dump


step0:
	bin/mk_data${RANK}
	bin/before${RANK}
	bin/klini${RANK}

step1:
	./merger ${DATADIR}/9-q-klini* > ${DATADIR}/9-q0
	rm ${DATADIR}/9-q-klini*

adopt: merger
	./merger ${DATADIR}/9-w* > 9-w
	./merger ${DATADIR}/9-b* > 9-b
	rm -rf ${DATADIR}/9-w* ${DATADIR}/9-b*
	mv 9-w 9-b ${DATADIR}

mergeq: merger
	./merger ${DATADIR}/9-q-qprocessor > ${DATADIR}/9-q
	rm ${DATADIR}/9-q-qprocessor

restart: shader
	skill shader
	./shader
