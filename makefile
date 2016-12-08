CC=gcc -g

all: compile

compile: control.o
	$(CC) -o control.out control.o

control.o: control.c
	$(CC) -c control.c

run: control.out
	./control.out

fast: compile run
