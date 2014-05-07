FIELD_OBJS= test.o field.o
CC=gcc
CFLAGS=-Wall
SOURCES= test.c field.c ../field.h

all: tests
tests: $(SOURCES) $(FIELD_OBJS) 
	$(CC) $(FIELD_OBJS) -o $@

clean:
	rm -f $(FIELD_OBJS) tests
