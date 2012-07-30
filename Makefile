all : xnet

xnet_objects = xnet.o



CXXFLAGS= -g -I/usr/X11R6/include -I/usr/local/include



xnet: $(xnet_objects)
	g++ -L/usr/local/lib -L/usr/X11R6/lib \
	-o xnet $(xnet_objects) -lX11


xnet.o : xnet.cpp

clean :
	rm -f *.o xnet
