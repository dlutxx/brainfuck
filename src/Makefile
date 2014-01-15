
a.out: brainfuck.o main.c
	gcc -Wall main.c brainfuck.o

brainfuck.o: brainfuck.h brainfuck.c
	gcc -Wall -c brainfuck.h brainfuck.c

.PHONY: clean
.SILENT: clean
clean:
	rm  -rf a.out brainfuck.o *.h.gch
