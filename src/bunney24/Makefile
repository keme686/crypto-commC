BUNNY_OBJS= test.o bunny24.o ../field/field.o 
CC=gcc
CFLAGS=-Wall
SOURCES= test.c bunny24.c ../field/field.c ../field.h ../bunny24.h

all: BUNNYtest
BUNNYtest: $(SOURCES) $(BUNNY_OBJS) 
	$(CC) $(BUNNY_OBJS) -o $@

clean:
	rm -f $(BUNNY_OBJS) BUNNYtest
