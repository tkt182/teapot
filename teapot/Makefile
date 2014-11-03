program = teapot
objs = teapot.o

CC = g++
CFLAGS = -I. -I/usr/include
LDFLAGS = -lglut -lGL

.SUFFIXES: .cpp .o

.PHONY: all
all: depend $(program)

$(program): $(objs)
	$(CC) -o $(program) $^

.cpp.o:
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	$(RM) $(program) $(objs) depend.inc

.PHONY: depend
depend: $(objs:.o=.cpp)
	-@ $(RM) depend.inc
	-@ for i in $^; do\
		cpp -MM $$i | sed "s/\ [_a-zA-Z0-9][_a-zA-Z0-9]*\.c//g" >> depend.inc;\
	done

-include depend.inc



