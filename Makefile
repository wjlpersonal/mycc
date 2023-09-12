SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)

mycc: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJS): mycc.h

clean:
	rm -f chibicc *.o *~ tmp*
