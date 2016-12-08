all: compile

compile:
	gcc -o control.out control.o

control.o: control.c
	gcc -c control.c

run: control.out
	./control.out

fast: compile run
