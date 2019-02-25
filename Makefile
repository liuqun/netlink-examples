CFLAGS ?= -g -O0
CFLAGS += -lmnl

PROGRAMS := dump-ip-addr

.PHONY: all
all: $(PROGRAMS)

.PHONY: clean
clean:
	$(RM) -f $(PROGRAMS) *.o
