CFLAGS += -m64
INCLUDEPATH  = -I/usr/include/GL/
LIBPATH = -L/usr/lib64 -L/usr/X11R6/lib
LDFLAGS+=  -lGL -lglut -lrt -lGLU -lX11 -lm  -lXmu -lXext -lXi
endif


CC = g++ -O3 -Wall $(INCLUDEPATH)


PROGS = modelflood interpbfe

default: $(PROGS)

modelflood: modelflood.o grid.o bfe.o flood.o 
	$(CC) -o $@ modelflood.o grid.o bfe.o flood.o $(LDFLAGS)

interpbfe: interpbfe.o bfe.o grid.o 
  $(CC) -o $@ interpbfe. bfe.o grid.o $(LDFLAGS)

modelflood.o: floodmain.cpp grid.hpp flood.hpp  
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) floodmain.cpp  -o $@

grid.o: grid.cpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) grid.cpp -o $@

bfe.o: bfe.cpp bfe.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) bfe.cpp -o $@

flood.o: flood.cpp flood.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) flood.cpp -o $@

interpbfe.o: interpbfe.cpp bfe.hpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) interpbfe.cpp -o $@



clean::
	rm *.o
	rm modelflood interpbfe
