SPONGE_OBJS= test.o sponge.o ../bunney24/bunny24.o ../field/field.o
CC=gcc
CFLAGS=-Wall
SOURCES= test.c sponge.c ../bunney24/bunny24.c ../bunny24.h ../sponge.h ../field/field.c ../field.h

all: SPONGEtest
SPONGEtest: $(SOURCES) $(SPONGE_OBJS) 
	$(CC) $(SPONGE_OBJS) -o $@

clean:
	rm -f $(SPONGE_OBJS) SPONGEtest
