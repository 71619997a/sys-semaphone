CC=gcc -g

all: compile

compile: control.o writer.o
	$(CC) -o control.out control.o
	$(CC) -o writer.out writer.o

control.o: control.c
	$(CC) -c control.c

writer.o: writer.c
	$(CC) -c writer.c

run: control.out
	./control.out

fast: compile run
