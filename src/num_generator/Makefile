All5_OBJS= prng.o ../bunney24/bunny24.o ../field/field.o  ../StreamCipher/All5/All5.o ../StreamCipher/Maj5/Maj5.o ../StreamCipher/A5_1/A5_1.o ../StreamCipher/lfsr/lfsr.o 
CC=gcc 
CFLAGS=-Wall
SOURCES= prng.c ../bunney24/bunny24.c ../StreamCipher/All5/All5.c ../StreamCipher/Maj5/Maj5.c ../StreamCipher/A5_1/A5_1.c ../StreamCipher/lfsr/lfsr.c ../StreamCipher/lfsr.h ../StreamCipher/A5_1.h ../StreamCipher/Maj5.h ../StreamCipher/All5.h ../bunny24.h ../field/field.c ../field.h 

all: prngtest
prngtest: $(SOURCES) $(All5_OBJS) 
	$(CC) $(All5_OBJS) -lcrypto -o $@

clean:
	rm -f $(All5_OBJS) prngtest
