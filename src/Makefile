
bf: brainfuck.o main.c
	gcc -Wall main.c brainfuck.o -o bf

brainfuck.o: brainfuck.h brainfuck.c
	gcc -Wall -c brainfuck.h brainfuck.c

.PHONY: clean
.SILENT: clean
clean:
	rm  -rf bf brainfuck.o
