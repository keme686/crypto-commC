All5_OBJS= correlationAttack.o All5/All5.o Maj5/Maj5.o A5_1/A5_1.o lfsr/lfsr.o 
CC=gcc
CFLAGS=-Wall
SOURCES= correlationAttack.c All5/All5.c Maj5/Maj5.c A5_1/A5_1.c lfsr/lfsr.c lfsr.h A5_1.h Maj5.h All5.h

all: correlation
correlation: $(SOURCES) $(All5_OBJS) 
	$(CC) $(All5_OBJS) -o $@

clean:
	rm -f $(All5_OBJS) correlation
