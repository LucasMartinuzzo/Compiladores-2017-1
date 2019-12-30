all: trab1
trab1: lex.yy.c
	gcc lex.yy.c -o trab1 -lfl
lex.yy.c: tiny.l
	flex -o lex.yy.c tiny.l
clean:
	rm -rf *.c *.out
