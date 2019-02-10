LIBS = -lXm -lXt -lX11

xmless:	xmless.c	
	cc -g -o  xmless xmless.c $(LIBS)

