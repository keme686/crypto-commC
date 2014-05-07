A5_OBJS= test.o Maj5.o ../A5_1/A5_1.o ../lfsr/lfsr.o 
CC=gcc
CFLAGS=-Wall
SOURCES= test.c Maj5.c ../A5_1/A5_1.c ../lfsr/lfsr.c ../lfsr.h ../A5_1.h ../Maj5.h

all: Maj5test
Maj5test: $(SOURCES) $(A5_OBJS) 
	$(CC) $(A5_OBJS) -o $@

clean:
	rm -f $(A5_OBJS) Maj5test
