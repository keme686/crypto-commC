A5_OBJS= test.o A5_1.o ../lfsr/lfsr.o 
CC=gcc
CFLAGS=-Wall
SOURCES= test.c A5_1.c ../lfsr/lfsr.c ../lfsr.h ../A5_1.h

all: A51test
A51test: $(SOURCES) $(A5_OBJS) 
	$(CC) $(A5_OBJS) -o $@

clean:
	rm -f $(A5_OBJS) A51test
