CC += -I.. -DDATADIR=\"$(realpath ../../data)/\"

INCS := ../sha.h ../twobit.h ../pack.h ../cnk.h
UTILS := pack cnk facecontrol ask itoa permut
UTILS += move
UTILS += show
#UTILS += pascal
UTILS += haszero
UTILS += qstat
UTILS += blist_test

all : ${UTILS}

${UTILS}: % :  %.c ${INCS} Makefile
	${CC} -o $@ $<

clean:
	rm -rf ${UTILS}
.gitignore:
	echo ${UTILS} | sed ss\ s\\nsg >> ../$@
