all: bison flex gcc
	@echo "Done."

bison: parser.y 
	bison parser.y --verbose

flex: scanner.l 
	flex scanner.l

gcc: scanner.c parser.c
	gcc -Wall -o trab2 scanner.c parser.c

clean:
	@rm -f *.o *.output scanner.c parser.h parser.c trab2
