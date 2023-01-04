#CC=gcc
#CFLAGS=-c -Wall

all: findtopk

findtopk: findtopk.o
    gcc findtopk.o -o findtopk

findtopk.o: findtopk.c
    gcc -c -Wall findtopk.c

run:
    ./findtopk 1 1 text0.txt asdf.txt

clean:
    \rm -rf *o findtopk

