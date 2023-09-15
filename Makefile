OBJS=*.cpp

mycc: $(OBJS)
	g++ -g -o $@ $^ 

$(OBJS): mycc.h

clean:
	rm *.o mycc
