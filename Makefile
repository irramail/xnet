all : xnet

CXXFLAGS= -g -I/usr/X11R6/include -I/usr/local/include



xnet: xnet.cpp
	g++ -L/usr/local/lib -L/usr/X11R6/lib \
	-o xnet xnet.cpp -lX11

clean :
	rm -f *.o xnet
