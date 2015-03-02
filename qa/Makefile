CC += -I.. -DDATADIR=\"$(realpath ../../data)/\"

INCS := ../sha.h ../twobit.h ../pack.h ../cnk.h
UTILS := pack cnk facecontrol ask itoa dump permut
UTILS += move
UTILS += show
#UTILS += pascal
UTILS += haszero
UTILS += qstat

all : ${UTILS}

${UTILS}: % :  %.c ${INCS} Makefile
	${CC} -o $@ $<

clean:
	rm -rf ${UTILS}
.gitignore:
	echo ${UTILS} | sed ss\ s\\nsg >> ../$@
