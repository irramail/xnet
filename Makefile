#all : xaux

#CXXFLAGS= -g -I/usr/X11R6/include -I/usr/local/include



#xaux: xaux.cpp
#	g++ -L/usr/local/lib -L/usr/X11R6/lib \
#	-o xaux xaux.cpp -lX11


#clean :
#	rm -f *.o xaux
GTK_INCLUDE = $(shell pkg-config --cflags gtk+-2.0)
GTK_LIB = -L/usr/local/lib
X11_LIB = -L/usr/X11R6/lib
CC = g++ -g -Wall
CXXFLAGS = $(GTK_INCLUDE)
LDFLAGS = $(GTK_LIB) $(X11_LIB) $(shell pkg-config --libs gtk+-2.0)

OBJS = xmx.o

xmx:	$(OBJS)
	$(CC) $(GTK_LIB) $(X11_LIB) $(OBJS) -o xmx $(LDFLAGS)

clean:
	rm -f *.o *~ xmx


