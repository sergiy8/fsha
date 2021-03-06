VERSION:=0.8.0 # eight U&R
export RANK ?= 9
export NPROC ?= $(shell grep -ic Processor /proc/cpuinfo)
export DATADIR ?= $(realpath ../data/)
export NODAMKA ?= 0
export MEGASK_REMOTE ?= 0
export CC ?= gcc
export WRANK_LIST := $(shell "seq" $$((${RANK}-1)))
export UTILS2

-include ${RANK}.mk
-include local.mk
MAKEFLAGS = -j $(NPROC)
CC += -Wall -DRANK=${RANK}
CC += -DDATADIR=\"${DATADIR}/\"
CC += -DNPROC=$(NPROC)
CC += -Wno-multichar
CC += -std=gnu99
CC += $(if ${WRANK},-DWRANK=${WRANK}) $(if ${NODAMKA},-DNODAMKA=${NODAMKA})

NVCC:= nvcc -Xptxas -v ${CUDA_GPU} -DRANK=${RANK}
CUDALIBS:= -L/usr/local/cuda/lib64 -L/usr/local/cuda/lib -lcuda -lcudart

INCS += sha.h arch.h twobit.h blist.h neighbor.h tprintf.h percent.h tpack.h remote.h
INCS += cnk.inc neighbor.inc megask.c move5.c malloc_file.c

# DB processors
UTILS := stat mk_data before klini after
UTILS := $(addsuffix ${RANK},${UTILS})
UTILS := $(addprefix bin/,${UTILS})


# helpers
UTILS2 += mk_blist mk_c16 mk_neighbor mk_cnk
UTILS2 += debut
UTILS2 += shader

UTILS3 := select
UTILS3 += asciiart
UTILS3 += known

CUTILS := mk_data before after

all: ${UTILS} wrankers
KLINIES := $(addprefix bin/klini${RANK}-,${WRANK_LIST})
STATS := $(addprefix bin/stat${RANK}-,${WRANK_LIST})
wrankers: ${KLINIES} ${STATS}

qa/% : qa/%.c
	${MAKE} -C qa $*
go:	${UTILS} ${KLINIES}
	./bin/mk_data${RANK}
	./bin//before${RANK}
	for i in `seq $$((${RANK}/2))` ; do ./bin/klini${RANK}-$$i ; done
	./bin//after${RANK}

${UTILS} : bin/%${RANK} :  %.cu main_multithread.c ${MAKEFILE_LIST} ${INCS}
	@mkdir -p bin
	${CC} -DIN_$* -include sha.h -include $< main_multithread.c -lpthread -o$@

${KLINIES}: bin/klini${RANK}-% : klini.cu main_multithread.c ${MAKEFILE_LIST} ${INCS}
	@mkdir -p bin
	${CC} -DIN_klini -DWRANK=$* -include sha.h -include klini.cu main_multithread.c -lpthread -o$@

${STATS}: bin/stat${RANK}-% : stat.cu main_multithread.c ${MAKEFILE_LIST} ${INCS}
	@mkdir -p bin
	${CC} -DIN_stat -DWRANK=$* -include sha.h -include stat.cu main_multithread.c -lpthread -o$@

${UTILS2} : % :  %.c ${MAKEFILE_LIST} ${INCS}
	${CC} $< -lpthread  -o$@

select: select.c ${MAKEFILE_LIST} select.inc ${INCS}
	${CC} -Iplugin_select -I. $< -lpthread -lreadline -o$@
select.inc: plugin_select ${MAKEFILE_LIST}
	ls $</*.c | awk '{print "#include","<"$$1">";}' > $@

NCURSES_LIBS := $(shell ncurses5-config --libs)
asciiart: asciiart.c forced.o ${MAKEFILE_LIST} ${INCS}
	${CC} -DMEGASK_REMOTE=${MEGASK_REMOTE} $< forced.o ${NCURSES_LIBS} -o$@
forced.o : forced.c ${MAKEFILE_LIST} ${INCS}
	${CC} -c $< -o$@

$(addprefix c,${CUTILS}) : c% : %.kernel cudamain.cpp cudablin/cudablin.h ${INCS}
	${NVCC} -DIN_$* -o $@ -include sha.h -include $*.kernel cudamain.cpp  ${CUDALIBS}

%.kernel: %.cubin
	bin2c -n cubin -st $< > $@
%.cubin : %.cu ${INCS}
	${NVCC} -DIN_$* --cubin -o $@ -include sha.h $<

CLEANLIST := ${UTILS2} ${UTILS3} $(addprefix c,${CUTILS}) *.cubin *.kernel
CLEANLIST += forced.o
clean:
	${MAKE} -C dbutil clean
	${MAKE} -C qa clean
	rm -rf $(CLEANLIST)
