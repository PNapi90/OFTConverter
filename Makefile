CC      = g++
CFLAGS  = -g -pthread -O3 -std=c++11 -Wall


OBJECTS = Parser.o

all: Program

%.o: %.cpp
	$(CC) -c $< $(CFLAGS)
	
Program: $(OBJECTS) Main.o
	$(CC) $(OBJECTS) Main.o -pthread -o OFTConversion

clean: 
	rm -f *.o $(PROGRAM)
	rm -f OFTConversion