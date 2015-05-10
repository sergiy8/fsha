CC += -I.. -DDATADIR=\"$(realpath ../../data)/\" -Wno-multichar

INCS := ../sha.h ../twobit.h ../pack.h ../cnk.inc
UTILS := pack facecontrol ask itoa permut
UTILS += move
UTILS += show
UTILS += haszero
UTILS += qstat
UTILS += blist_test

all : ${UTILS}

${UTILS}: % :  %.c ${INCS} Makefile
	${CC} -o $@ $<

clean:
	rm -rf ${UTILS}
.gitignore: Makefile
	echo ${UTILS} | sed ss\ s\\nsg > $@
