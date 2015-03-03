VERSION:=0.7.0 # seven U&R
NPROC := $(shell grep -ic Processor /proc/cpuinfo)
MAKEFLAGS = -j $(NPROC)
RANK ?= 7
#WRANK=3
include cnk.mak

export DATADIR := $(realpath ../data/)

export CC := gcc -Wall -DRANK=${RANK} -DCNK=${CNK} $(if ${WRANK},-DWRANK=${WRANK})
CC += -DDATADIR=\"${DATADIR}/\"
CC += -DDEBUG
#CC += -DREVERSE
CC += -DNPROC=$(NPROC)



-include cudablin/cuda_gpu.mk
NVCC:= nvcc -Xptxas -v ${CUDA_GPU} -DRANK=${RANK} -DCNK=${CNK}
CUDALIBS:= -L/usr/local/cuda/lib64 -L/usr/local/cuda/lib -lcuda -lcudart

INCS := sha.h arch.h twobit.h cnk.h pack.h blist.h neighbor.h tprintf.h percent.h
INCS += neighbor.inc move4.c ask.c malloc_file.c
UTILS := stat mk_data klini before after
UTILS2 := mk_blist mk_c16 mk_pascal mk_neighbor
UTILS2 += solver
UTILS2 += debut
CUTILS := mk_data klini before after

all: ${UTILS}
qa/% : qa/%.c
	${MAKE} -C qa $*
go:	${UTILS} ${DATADIR}/blist
	./mk_data
	./before
	./klini
	./after

${DATADIR}blist: mk_blist
	./$<

${UTILS} : % :  %.cu main_multithread.c Makefile ${INCS}
	${CC} -DIN_$* -o $@ -include sha.h -include $< main_multithread.c -lpthread

${UTILS2} : % :  %.c Makefile ${INCS}
	${CC} -o $@  $< -lpthread

$(addprefix c,${CUTILS}) : c% : %.kernel cudamain.cpp cudablin/cudablin.h ${INCS}
	${NVCC} -DIN_$* -o $@ -include sha.h -include $*.kernel cudamain.cpp  ${CUDALIBS}

%.kernel: %.cubin
	bin2c -n cubin -st $< > $@
%.cubin : %.cu ${INCS}
	${NVCC} -DIN_$* --cubin -o $@ -include sha.h $<

CLEANLIST := ${UTILS} ${UTILS2} $(addprefix c,${CUTILS}) *.cubin *.kernel
clean:
	${MAKE} -C dbutil clean
	${MAKE} -C qa clean
	rm -rf $(CLEANLIST)

PROJDIR := $(shell basename ${CURDIR})
tar:
	cd .. ; tar -zcvf ${PROJDIR}${VERSION}.tgz ${PROJDIR}
.gitignore: Makefile
	echo $(CLEANLIST) | sed ss\ s\\nsg >$@
	echo c16.inc >> $@
	${MAKE} -C dbutil .gitignore
	${MAKE} -C qa .gitignore
