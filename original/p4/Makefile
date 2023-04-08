# UNIX Makefile
# Makefile for the P4 pascal compiler and interpreter

CC = gcc
CFLAGS = -O3 -fomit-frame-pointer -funroll-loops
#CFLAGS = -Wall -g

.c.o:
	$(CC) -I. $(CFLAGS) -c $<

all: pcom pint

pcom: libp2c.a pcom.o 
	$(CC) pcom.o -o pcom -L. -lp2c

pcom.o: pcom.c p2c.h

pint: libp2c.a pint.o
	$(CC) pint.o -o pint -L. -lp2c -lm

pint.o: pint.c p2c.h

libp2c.a: p2clib.o
	ar rv libp2c.a p2clib.o
	ranlib libp2c.a

p2clib.o: p2clib.c p2c.h

clean:
	$(RM) *% *.o *.a pint pcom core prr prd
