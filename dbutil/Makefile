UTILS:= dbcreat dbask dblist
CFLAGS += -I.. -DDATADIR=\"$(realpath ../../data)/\"
CFLAGS += -DWITH_MAIN
$(info CFLAGS = $(CFLAGS))
all: $(UTILS)
$(UTILS) : % : %.c
	cc $(CFLAGS) -Wall -o$@ $<
clean:
	rm -rf $(UTILS)
.gitignore:
	echo $(UTILS) | sed ss\ s\\nsg >> ../$@
