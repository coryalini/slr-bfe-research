PLATFORM = $(shell uname)


## Compilation flags
##comment out one or the other 
##debugging 
CFLAGS = -g 
##release
#CFLAGS = -O3 -DNDEBUG
LDFLAGS=

CFLAGS+= -Wall


ifeq ($(PLATFORM),Darwin)
## Mac OS X
CFLAGS += -m64 -isystem/usr/local/include -Wno-deprecated
LDFLAGS+= -m64 -lc -framework AGL -framework OpenGL -framework GLUT -framework Foundation
else
## Linux
CFLAGS += -m64 -Wno-deprecated
INCLUDEPATH  = -I/usr/include/GL/ 
LIBPATH = -L/usr/lib64 -L/usr/X11R6/lib
LDFLAGS+=  -lGL -lglut -lrt -lGLU -lX11 -lm  -lXmu -lXext -lXi
endif


CC = g++ -O3 -Wall $(INCLUDEPATH)


PROGS = interp modelflood renderflood

default: $(PROGS)

interp: interpmain.o interpolate.o grid.o 
	$(CC) -o $@ interpmain.o interpolate.o grid.o $(LDFLAGS)

modelflood: floodmain.o flood.o grid.o 
	$(CC) -o $@ floodmain.o grid.o flood.o $(LDFLAGS)

renderflood: renderFlood.o grid.o
	$(CC) -o $@ renderFlood.o grid.o $(LDFLAGS)

#########
renderFlood.o:	renderFlood.cpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) renderFlood.cpp  -o $@


renderflood: renderflood.o grid.o 
	$(CC) -o $@ renderflood.o grid.o $(LDFLAGS)


renderflood.o: renderFlood.cpp grid.hpp 
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) renderFlood.cpp  -o $@

floodmain.o: floodmain.cpp grid.hpp flood.hpp  
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) floodmain.cpp  -o $@

flood.o: flood.cpp flood.hpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) flood.cpp -o $@

grid.o: grid.cpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) grid.cpp -o $@

interpolate.o: interpolate.cpp interpolate.hpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) interpolate.cpp -o $@


interpmain.o: interpmain.cpp interpolate.hpp grid.hpp
	$(CC) -c $(INCLUDEPATH) $(CFLAGS) interpmain.cpp -o $@



clean::
	rm *.o
	rm modelflood interp renderflood
