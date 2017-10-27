SRC = $(wildcard *.cpp)
HEADER = $(wildcard *.h)
GCC = gcc

all: $(SRC) $(HEADER)
	g++ -Wall -std=c++11 -O3 -c $(SRC)
	g++ -Wall -std=c++11 -O3 *.o -o main
	
_PHONE: clean

clean:
	rm *.o *.s main
