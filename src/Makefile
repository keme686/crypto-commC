
STREAMCIPHER	= $(patsubst %.c, %.o, $(wildcard StreamCipher/*.c))
RSA		= $(patsubst %.c, %.o, $(wildcard rsa/*.c))
BUNNY           = $(patsubst %.c, %.o, $(wildcard bunney24/*.c))
SPONGE          = $(patsubst %.c, %.o, $(wildcard sponge/*.c))
PRNG		= $(patsubst %.c, %.o, $(wildcard num_generator/*.c))
SERVER_OBJS=server.o common.o
CLIENT_OBJS=client.o common.o 
KEYGEN_OBJ=generateKey.o
CC=gcc
CFLAGS=-Wall

all: server client keygen

client: $(CLIENT_OBJS) $(PRNG) $(STREAMCIPHER) $(BUNNY) $(SPONGE) $(RSA)
	$(CC) $(CLIENT_OBJS) -o $@ $(PRNG) $(STREAMCIPHER) $(BUNNY) $(SPONGE) $(RSA) -lcrypto

server: $(SERVER_OBJS) $(PRNG) $(STREAMCIPHER) $(BUNNY) $(SPONGE) $(RSA)
	$(CC) $(SERVER_OBJS) -o $@ $(PRNG) $(STREAMCIPHER) $(BUNNY) $(SPONGE) $(RSA) -lcrypto

keygen: $(KEYGEN_OBJ) $(PRNG) $(BUNNY)  $(STREAMCIPHER)
	$(CC) $(KEYGEN_OBJ) -o $@ $(PRNG) $(BUNNY) $(STREAMCIPHER) -lcrypto

clean:
	rm -f *.o $(PRNG) $(STREAMCIPHER) $(BUNNY) $(SPONGE) $(RSA) $(CLIENT_OBJS) $(SERVER_OBJS) $(KEYGEN_OBJ) server client keygen
