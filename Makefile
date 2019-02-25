CFLAGS ?= -g -O0

PROGRAMS := dump-ip-addr
PROGRAMS += dump-interface-list

vpath %.c src

.PHONY: all
all: $(PROGRAMS)

RM ?= rm -f
RMDIR := $(RM) -r

.PHONY: clean
clean:
	$(RM) $(PROGRAMS) *.o
