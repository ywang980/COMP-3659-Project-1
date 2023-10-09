default: myshell

all: clean myshell

myshell: String.o Heap.o Shell.o CommandLine.o
	gcc String.o Heap.o Shell.o CommandLine.o -o myshell

String.o: String.c String.h
	gcc -c String.c

Heap.o: Heap.c Heap.h
	gcc -c Heap.c

Shell.o: Shell.c Shell.h
	gcc -c Shell.c

CommandLine.o: CommandLine.c CommandLine.h
	gcc -c CommandLine.c

clean:
	rm *.o myshell
