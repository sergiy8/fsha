VERSION:=0.7.0 # seven U&R
NPROC ?= $(shell grep -ic Processor /proc/cpuinfo)
MAKEFLAGS = -j $(NPROC)
RANK ?= 8
ifeq (${RANK},9)
WRANK ?= 1
endif
export DATADIR := $(realpath ../data/)

export CC := gcc -Wall -DRANK=${RANK} $(if ${WRANK},-DWRANK=${WRANK})
CC += -march=native -Ofast
CC += -DDATADIR=\"${DATADIR}/\"
#CC += -DDEBUG
CC += -DNPROC=$(NPROC)



-include cudablin/cuda_gpu.mk
NVCC:= nvcc -Xptxas -v ${CUDA_GPU} -DRANK=${RANK}
CUDALIBS:= -L/usr/local/cuda/lib64 -L/usr/local/cuda/lib -lcuda -lcudart

INCS := sha.h arch.h twobit.h pack.h blist.h neighbor.h tprintf.h percent.h
INCS += cnk.inc neighbor.inc megask.inc move4.c ask.c malloc_file.c

# DB processors
UTILS := stat mk_data before after
UTILS := $(addsuffix ${RANK},${UTILS})
UTILS := $(addprefix bin/,${UTILS})


# helpers
UTILS2 := mk_blist mk_c16 mk_neighbor mk_cnk
UTILS2 += solver
UTILS2 += debut

UTILS3 := select
UTILS3 += asciiart

CUTILS := mk_data before after

ifneq (${RANK},9)
WRANK_LIST := $(shell "seq" $$((${RANK}-1)))
else
ifeq (${WRANK},)
$(error WRANK should be set in WFILE mode)
endif
WRANK_LIST:= ${WRANK}
endif
$(info WRANK_LIST=${WRANK_LIST})

all: ${UTILS} klinies
KLINIES := $(addprefix bin/klini${RANK}-,${WRANK_LIST})
klinies: ${KLINIES}

qa/% : qa/%.c
	${MAKE} -C qa $*
go:	${UTILS} ${KLINIES}
	./bin/mk_data${RANK}
	./bin//before${RANK}
	for i in `seq $$((${RANK}/2))` ; do ./bin/klini${RANK}-$$i ; done
	./bin//after${RANK}

${DATADIR}blist: mk_blist
	./$<

${UTILS} : bin/%${RANK} :  %.cu main_multithread.c Makefile ${INCS}
	@mkdir -p bin
	${CC} -DIN_$* -include sha.h -include $< main_multithread.c -lpthread -o$@

${KLINIES}: bin/klini${RANK}-% : klini.cu main_multithread.c Makefile ${INCS}
	@mkdir -p bin
	${CC} -DIN_klini -DWRANK=$* -include sha.h -include klini.cu main_multithread.c -lpthread -o$@

${UTILS2} : % :  %.c Makefile ${INCS}
	${CC} $< -lpthread  -o$@

select: select.c Makefile select.inc ${INCS}
	${CC} -Iplugin_select -I. $< -lpthread -lreadline -o$@
select.inc: plugin_select Makefile
	ls $</*.c | awk '{print "#include","<"$$1">";}' > $@

NCURSES_LIBS := $(shell ncurses5-config --libs)
asciiart: asciiart.c Makefile ${INCS}
	${CC} -Wall $< ${NCURSES_LIBS} -o$@

$(addprefix c,${CUTILS}) : c% : %.kernel cudamain.cpp cudablin/cudablin.h ${INCS}
	${NVCC} -DIN_$* -o $@ -include sha.h -include $*.kernel cudamain.cpp  ${CUDALIBS}

%.kernel: %.cubin
	bin2c -n cubin -st $< > $@
%.cubin : %.cu ${INCS}
	${NVCC} -DIN_$* --cubin -o $@ -include sha.h $<

CLEANLIST := ${UTILS2} ${UTILS3} $(addprefix c,${CUTILS}) *.cubin *.kernel
clean:
	${MAKE} -C dbutil clean
	${MAKE} -C qa clean
	rm -rf $(CLEANLIST)

PROJDIR := $(shell basename ${CURDIR})
tar:
	cd .. ; tar -zcvf ${PROJDIR}${VERSION}.tgz ${PROJDIR}
