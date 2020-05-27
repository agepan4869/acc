CFLAGS=-std=c11 -g -static -fno-common
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

acc: $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS)

$(OBJS): acc.h

test: acc
	./test.sh

clean:
	rm -f acc *o *~ tmp*

.PHONY: test clean