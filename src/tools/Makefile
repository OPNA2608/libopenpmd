CC   ?= gcc
PROG ?= pmd_instrip

SRCS = pmd_instrip.c
OBJS = $(SRCS:.c=.o)

CFLAGS  = -g -ansi
CFLAGS += -Wall -Wextra -Werror -pedantic -pedantic-errors

LDFLAGS =

.PHONY: all clean

all: $(PROG)

clean:
	rm -fv $(PROG) $(OBJS)

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(PROG)

$(OBJS): $(SRCS)
	$(CC) $(CFLAGS) -c $(@:.o=.c) -o $@

