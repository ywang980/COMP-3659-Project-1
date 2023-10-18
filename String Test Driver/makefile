default: teststring

all: clean teststring

teststring: String.o Heap.o StringTester.o
	gcc String.o Heap.o StringTester.o -o teststring

String.o: String.c String.h
	gcc -c String.c

Heap.o: Heap.c Heap.h
	gcc -c Heap.c

StringTester.o: StringTester.c
	gcc -c StringTester.c

clean:
	rm *.o teststring
