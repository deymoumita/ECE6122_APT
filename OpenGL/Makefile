# from website
# Makefile for the icosahedron assignmnet
# George F. Riley, ECE8893 Fall 2011

all:	circle circle-complete

circle:	circle.o
	$(CXX) -g -o circle circle.o -L/usr/lib64 -lglut -lGLU -lGL

circle.o:	circle.cc
	$(CXX) -c -g circle.cc

circle-complete:	circle-complete.o
	$(CXX) -g -o circle-complete circle-complete.o -L/usr/lib64 -lglut -lGLU -lGL

circle-complete.o:	circle-complete.cc
	$(CXX) -c -g circle-complete.cc

clean:
	@rm -f circle-complete circle-complete.o
