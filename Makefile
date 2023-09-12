SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

mycc: $(OBJS)
	g++  -o $@ $^ $(LDFLAGS)

$(OBJS): mycc.h

clean:
	del -rf *.o
