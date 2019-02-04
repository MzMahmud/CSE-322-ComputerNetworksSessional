CC=g++ -std=c++11
CFlags=-Wall -Werror

all: run

run: main.out
	./main.out

main.out: main.o crc.o hamming.o
	$(CC)    $(CFlags) main.o crc.o hamming.o -o main.out

main.o: main.cpp defs.h
	$(CC) -c $(CFlags) main.cpp -o main.o

crc.o: crc_proc.cpp defs.h
	$(CC) -c $(CFlags) crc_proc.cpp -o crc.o

hamming.o: hamming_proc.cpp defs.h
	$(CC) -c $(CFlags) hamming_proc.cpp -o hamming.o

clean:
	rm -rf *.out *.o
