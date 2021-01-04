CC=gcc
CFLAGS=-g -Wall
SRC=myshell.c lex.yy.c

all:
	flex parsecmd.l
	$(CC) $(CFLAGS) -o myshell $(SRC) -lfl -lreadline -ltermcap

clean:
	rm -f myshell myshell.o lex.yy.c
