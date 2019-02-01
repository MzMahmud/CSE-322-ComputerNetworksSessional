all: run

run: main.out
	./main.out

main.out: main.o crc.o hamming.o
	g++ -std=c++11 main.o crc.o hamming.o -o main.out

main.o: main.cpp defs.h
	g++ -std=c++11 -c main.cpp -o main.o

crc.o: crc_proc.cpp defs.h
	g++ -std=c++11 -c crc_proc.cpp -o crc.o

hamming.o: hamming_proc.cpp defs.h
	g++ -std=c++11 -c hamming_proc.cpp -o hamming.o

clean:
	rm -rf *.out *.o
